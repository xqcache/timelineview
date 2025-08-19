#include "playbackvideoplayer.h"
#include <QDebug>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

struct PlaybackVideoPlayerPrivate {
    AVFormatContext* fmt_ctx { nullptr };
    SwsContext* sws_ctx { nullptr };
    AVCodecContext* codec_ctx { nullptr };
    AVPacket* packet { nullptr };
    AVFrame* frame { nullptr };
    AVFrame* rgb_frame { nullptr };
    QSize size;
    uint8_t* rgb_buffer { nullptr };
    int video_stream_idx = -1;
    qint64 frame_count = 0;
    double fps { 1 };

    // 播放控制
    qint64 frame_step { 1 };
    qint64 frame_index = 0;
    std::stop_source stop_source;
    std::unique_ptr<std::jthread> thread;
    std::mutex wait_mutex;

    // 图像缓存
    std::mutex image_mutex;
    QImage current_image;
    bool has_valid_frame = false;

    // 简化的状态跟踪
    qint64 last_frame = -1;
    bool is_playing = false;
};

PlaybackVideoPlayer::PlaybackVideoPlayer()
    : d_(new PlaybackVideoPlayerPrivate)
{
}

PlaybackVideoPlayer::~PlaybackVideoPlayer() noexcept
{
    close();
    delete d_;
}

bool PlaybackVideoPlayer::open(const QString& path)
{
    if (d_->fmt_ctx) {
        close();
    }

    if (avformat_open_input(&d_->fmt_ctx, path.toStdString().c_str(), nullptr, nullptr) != 0 || avformat_find_stream_info(d_->fmt_ctx, nullptr) < 0) {
        return false;
    }
    // 查找视频流
    for (unsigned i = 0; i < d_->fmt_ctx->nb_streams && d_->video_stream_idx == -1; i++) {
        if (d_->fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            d_->video_stream_idx = i;
            break;
        }
    }
    if (d_->video_stream_idx == -1) {
        return false;
    }
    AVStream* video_stream = d_->fmt_ctx->streams[d_->video_stream_idx];
    AVCodecParameters* codec_par = video_stream->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    d_->codec_ctx = codec ? avcodec_alloc_context3(codec) : nullptr;

    if (!d_->codec_ctx || avcodec_parameters_to_context(d_->codec_ctx, codec_par) < 0 || avcodec_open2(d_->codec_ctx, codec, nullptr) < 0) {
        return false;
    }

    // 计算视频总帧数和时长
    int64_t duration = d_->fmt_ctx->duration;
    if (duration == AV_NOPTS_VALUE) {
        duration = video_stream->duration * av_q2d(video_stream->time_base) * AV_TIME_BASE;
    }
    d_->fps = av_q2d(video_stream->r_frame_rate);
    d_->frame_count = (duration * d_->fps) / AV_TIME_BASE;
    d_->size = { d_->codec_ctx->width, d_->codec_ctx->height };

    // 初始化转换器
    d_->sws_ctx = sws_getContext(d_->size.width(), d_->size.height(), d_->codec_ctx->pix_fmt, d_->size.width(), d_->size.height(), AV_PIX_FMT_RGB24,
        SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    if (!d_->sws_ctx) {
        return false;
    }
    // 分配帧和缓冲区
    d_->packet = av_packet_alloc();
    d_->frame = av_frame_alloc();
    d_->rgb_frame = av_frame_alloc();

    int rgb_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, d_->size.width(), d_->size.height(), 1);
    d_->rgb_buffer = (uint8_t*)av_malloc(rgb_buffer_size);
    av_image_fill_arrays(d_->rgb_frame->data, d_->rgb_frame->linesize, d_->rgb_buffer, AV_PIX_FMT_RGB24, d_->size.width(), d_->size.height(), 1);

    return true;
}

void PlaybackVideoPlayer::close()
{
    d_->stop_source.request_stop();
    if (d_->thread) {
        d_->thread->join();
        d_->thread.reset();
    }

    {
        std::lock_guard<std::mutex> guard(d_->image_mutex);
        d_->current_image = QImage();
        d_->has_valid_frame = false;
    }
    d_->last_frame = -1;
    d_->is_playing = false;

    if (d_->rgb_buffer) {
        av_free(d_->rgb_buffer);
        d_->rgb_buffer = nullptr;
    }
    if (d_->rgb_frame) {
        av_frame_free(&d_->rgb_frame);
        d_->rgb_frame = nullptr;
    }
    if (d_->frame) {
        av_frame_free(&d_->frame);
        d_->frame = nullptr;
    }
    if (d_->packet) {
        av_packet_free(&d_->packet);
        d_->packet = nullptr;
    }
    if (d_->sws_ctx) {
        sws_freeContext(d_->sws_ctx);
        d_->sws_ctx = nullptr;
    }
    if (d_->codec_ctx) {
        avcodec_free_context(&d_->codec_ctx);
        d_->codec_ctx = nullptr;
    }
    if (d_->fmt_ctx) {
        avformat_close_input(&d_->fmt_ctx);
        d_->fmt_ctx = nullptr;
    }
    d_->video_stream_idx = -1;
    d_->frame_count = 0;
    d_->size = QSize();
}

QSize PlaybackVideoPlayer::size() const
{
    return d_->size;
}

bool PlaybackVideoPlayer::play(qint64 frame_step)
{
    if (!d_->rgb_frame)
        return false;

    if (d_->thread) {
        d_->stop_source.request_stop();
        d_->thread->join();
    }
    if (d_->stop_source.stop_requested()) {
        d_->stop_source = std::stop_source();
    }

    d_->frame_index = frame_step > 0 ? 0 : d_->frame_count;
    d_->frame_step = frame_step;
    d_->is_playing = true;
    d_->thread = std::make_unique<std::jthread>(&PlaybackVideoPlayer::run, this, d_->stop_source.get_token());
    return true;
}

void PlaybackVideoPlayer::pause()
{
    d_->stop_source.request_stop();
    if (d_->thread) {
        d_->thread->join();
        d_->thread.reset();
    }
    d_->is_playing = false;
}

void PlaybackVideoPlayer::stop()
{
    pause();
    d_->frame_index = 0;
}

bool PlaybackVideoPlayer::seekToFrame(qint64 frame_no)
{
    if (frame_no < 0 || frame_no >= d_->frame_count)
        return false;

    QImage new_frame = decodeFrame(frame_no);
    if (!new_frame.isNull()) {
        std::lock_guard<std::mutex> guard(d_->image_mutex);
        d_->current_image = new_frame;
        d_->has_valid_frame = true;
        d_->frame_index = frame_no;
        return true;
    }
    return false;
}

bool PlaybackVideoPlayer::isPlaying() const
{
    return d_->is_playing && d_->thread && !d_->stop_source.stop_requested();
}

qint64 PlaybackVideoPlayer::getCurrentFrame() const
{
    return d_->frame_index;
}

qint64 PlaybackVideoPlayer::getTotalFrames() const
{
    return d_->frame_count;
}

void PlaybackVideoPlayer::preloadFrame(qint64 frame_no)
{
    if (frame_no >= 0 && frame_no < d_->frame_count)
        decodeFrame(frame_no);
}
bool PlaybackVideoPlayer::hasValidFrame() const
{
    std::lock_guard<std::mutex> guard(d_->image_mutex);
    return d_->has_valid_frame;
}
void PlaybackVideoPlayer::clearFrameCache()
{
    d_->last_frame = -1;
}

void PlaybackVideoPlayer::run(std::stop_token st)
{
    qint64 duration = 1000.0 / d_->fps;
    std::chrono::steady_clock::time_point clock;

    auto wait = [this, duration, &clock, &st] {
        std::unique_lock<std::mutex> lock(d_->wait_mutex);
        std::condition_variable_any().wait_until(lock, st, clock + std::chrono::milliseconds(duration), [&st] { return st.stop_requested(); });
    };

    while (!st.stop_requested()) {
        clock = std::chrono::steady_clock::now();
        if (d_->frame_index >= 0 && d_->frame_index <= d_->frame_count) {
            QImage new_frame = decodeFrame(d_->frame_index);
            if (!new_frame.isNull()) {
                std::lock_guard<std::mutex> guard(d_->image_mutex);
                d_->current_image = new_frame;
                d_->has_valid_frame = true;
            }
        }
        d_->frame_index += d_->frame_step;
        wait();
    }
    d_->is_playing = false;
}

QImage PlaybackVideoPlayer::decodeFrame(qint64 frame_no) const
{
    if (!d_->fmt_ctx || !d_->codec_ctx || !d_->sws_ctx || frame_no < 0)
        return QImage();

    // 简单的seek策略：只在必要时seek
    if (abs(frame_no - d_->last_frame) > 1) {
        int64_t timestamp = (frame_no * AV_TIME_BASE) / d_->fps;
        if (av_seek_frame(d_->fmt_ctx, -1, timestamp, AVSEEK_FLAG_BACKWARD) < 0) {
            return QImage();
        }
        avcodec_flush_buffers(d_->codec_ctx);
    }

    // 解码帧
    for (int attempts = 0; attempts < 20; attempts++) {
        if (av_read_frame(d_->fmt_ctx, d_->packet) < 0)
            break;

        if (d_->packet->stream_index == d_->video_stream_idx) {
            if (avcodec_send_packet(d_->codec_ctx, d_->packet) == 0) {
                if (avcodec_receive_frame(d_->codec_ctx, d_->frame) == 0) {
                    if (sws_scale(d_->sws_ctx, d_->frame->data, d_->frame->linesize, 0, d_->codec_ctx->height, d_->rgb_frame->data, d_->rgb_frame->linesize)
                        > 0) {

                        QImage img(d_->rgb_frame->data[0], d_->size.width(), d_->size.height(), d_->rgb_frame->linesize[0], QImage::Format_RGB888);

                        if (!img.isNull()) {
                            d_->last_frame = frame_no;
                            av_packet_unref(d_->packet);
                            return img.copy();
                        }
                    }
                }
            }
        }
        av_packet_unref(d_->packet);
    }

    return QImage();
}

QImage PlaybackVideoPlayer::getImage() const
{
    std::lock_guard<std::mutex> guard(d_->image_mutex);
    return d_->current_image;
}