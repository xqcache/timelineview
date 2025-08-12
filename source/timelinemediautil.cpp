#include "timelinemediautil.h"
#include "timelinedef.h"
#include <QCoreApplication>
#include <QPainter>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}

namespace tl {

std::optional<TimelineMediaUtil::VideoInfo> TimelineMediaUtil::loadVideo(const QString& path)
{
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, path.toStdString().c_str(), nullptr, nullptr) != 0) {
        TL_LOG_ERROR("Failed to open media file: {}", path.toStdString());
        return std::nullopt;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        TL_LOG_ERROR("Failed to find stream info: {}", path.toStdString());
        avformat_close_input(&fmt_ctx);
        return std::nullopt;
    }

    // 查找视频流
    int video_stream_idx = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    if (video_stream_idx == -1) {
        TL_LOG_ERROR("No video stream found in media file: {}", path.toStdString());
        return std::nullopt;
    }

    AVStream* video_stream = fmt_ctx->streams[video_stream_idx];

    VideoInfo info;
    info.path = path;
    info.size.setWidth(video_stream->codecpar->width);
    info.size.setHeight(video_stream->codecpar->height);

    // 获取视频帧率
    if (video_stream->r_frame_rate.den != 0) {
        info.fps = static_cast<double>(video_stream->r_frame_rate.num) / video_stream->r_frame_rate.den;
    } else {
        info.fps = 25.0; // 默认帧率
    }

    // 获取视频时长(单位：ms)，优先使用容器时长
    if (fmt_ctx->duration != AV_NOPTS_VALUE) {
        info.duration = static_cast<double>(fmt_ctx->duration) / AV_TIME_BASE * 1000.0;
    } else if (video_stream->duration != AV_NOPTS_VALUE) {
        info.duration = video_stream->duration * av_q2d(video_stream->time_base) * 1000.0;
    } else {
        avformat_close_input(&fmt_ctx);
        return std::nullopt;
    }

    info.frame_count = static_cast<int>(info.duration * info.fps / 1000.0);
    avformat_close_input(&fmt_ctx);

    return info;
}

std::optional<TimelineMediaUtil::AudioInfo> TimelineMediaUtil::loadAudio(const QString& path, double fps)
{
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, path.toStdString().c_str(), nullptr, nullptr) != 0) {
        TL_LOG_ERROR("Failed to open media file: {}", path.toStdString());
        return std::nullopt;
    }

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        TL_LOG_ERROR("Failed to find stream info: {}", path.toStdString());
        avformat_close_input(&fmt_ctx);
        return std::nullopt;
    }

    int audio_stream_idx = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream_idx = i;
            break;
        }
    }

    if (audio_stream_idx == -1) {
        TL_LOG_ERROR("No audio stream found in media file: {}", path.toStdString());
        avformat_close_input(&fmt_ctx);
        return std::nullopt;
    }

    AudioInfo info;
    info.path = path;

    // 获取音频时长，优先使用容器时长
    if (fmt_ctx->duration != AV_NOPTS_VALUE) {
        info.duration = static_cast<double>(fmt_ctx->duration) / AV_TIME_BASE * 1000.0; // 转换为毫秒
    } else {
        // 备选方案：使用音频流时长
        AVStream* audio_stream = fmt_ctx->streams[audio_stream_idx];
        if (audio_stream->duration != AV_NOPTS_VALUE) {
            info.duration = audio_stream->duration * av_q2d(audio_stream->time_base) * 1000.0;
        } else {
            avformat_close_input(&fmt_ctx);
            return std::nullopt;
        }
    }

    info.frame_count = static_cast<int>(info.duration * fps / 1000.0);
    avformat_close_input(&fmt_ctx);
    return info;
}

QString TimelineMediaUtil::mediaInfoString(const VideoInfo& info)
{
    return QCoreApplication::translate("TimelineMediaUtil", "Path: %1\nSize: %2x%3\nFPS: %4\nDuration: %5ms\nFrame Count: %6")
        .arg(info.path.isEmpty() ? "N/A" : info.path)
        .arg(info.size.width())
        .arg(info.size.height())
        .arg(info.fps)
        .arg(info.duration, 0, 'f', 2)
        .arg(info.frame_count);
}

QString TimelineMediaUtil::audioInfoString(const AudioInfo& info)
{
    return QCoreApplication::translate("TimelineMediaUtil", "Path: %1\nDuration: %2ms\nFrame Count: %3")
        .arg(info.path.isEmpty() ? "N/A" : info.path)
        .arg(info.duration, 0, 'f', 2)
        .arg(info.frame_count);
}

QList<int16_t> TimelineMediaUtil::loadAudioWaveform(const QString& path)
{
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, path.toStdString().c_str(), nullptr, nullptr) < 0 || avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        if (fmt_ctx)
            avformat_close_input(&fmt_ctx);
        return {};
    }

    // 查找音频流
    int audio_stream = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams && audio_stream == -1; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream = i;
        }
    }

    if (audio_stream == -1) {
        avformat_close_input(&fmt_ctx);
        return {};
    }

    // 初始化解码器
    AVCodecParameters* codecpar = fmt_ctx->streams[audio_stream]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);
    AVCodecContext* codec_ctx = codec ? avcodec_alloc_context3(codec) : nullptr;

    if (!codec_ctx || avcodec_parameters_to_context(codec_ctx, codecpar) < 0 || avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        if (codec_ctx)
            avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return {};
    }

    int sample_rate = codec_ctx->sample_rate;

    // 初始化重采样器
    AVChannelLayout mono_layout = AV_CHANNEL_LAYOUT_MONO;
    SwrContext* swr_ctx = nullptr;
    if (swr_alloc_set_opts2(&swr_ctx, &mono_layout, AV_SAMPLE_FMT_S16, sample_rate, &codecpar->ch_layout, codec_ctx->sample_fmt, sample_rate, 0, nullptr) < 0
        || swr_init(swr_ctx) < 0) {
        swr_free(&swr_ctx);
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return {};
    }

    QList<int16_t> pcm_data;
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    // 解码音频帧
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == audio_stream && avcodec_send_packet(codec_ctx, pkt) == 0) {
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                int dst_samples = av_rescale_rnd(frame->nb_samples, sample_rate, codec_ctx->sample_rate, AV_ROUND_UP);

                uint8_t* dst_data = nullptr;
                int dst_linesize;
                if (av_samples_alloc(&dst_data, &dst_linesize, 1, dst_samples, AV_SAMPLE_FMT_S16, 0) >= 0) {
                    int converted = swr_convert(swr_ctx, &dst_data, dst_samples, (const uint8_t**)frame->data, frame->nb_samples);
                    if (converted > 0) {
                        int16_t* samples = (int16_t*)dst_data;
                        for (int i = 0; i < converted; ++i) {
                            pcm_data.append(samples[i]);
                        }
                    }
                    av_freep(&dst_data);
                }
            }
        }
        av_packet_unref(pkt);
    }

    // 清理资源
    av_frame_free(&frame);
    av_packet_free(&pkt);
    swr_free(&swr_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return pcm_data;
}

QImage TimelineMediaUtil::drawWaveform(const QList<int16_t>& pcm_data, int view_width, int width, int height)
{
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::black);

    if (pcm_data.isEmpty())
        return image;

    QPainter painter(&image);
    painter.setPen(Qt::green);

    int samples_per_pixel = qMax(1, pcm_data.size() / width);
    int mid_y = height / 2;

    for (int x = 0; x < view_width; ++x) {
        int start = x * samples_per_pixel;
        int end = qMin(start + samples_per_pixel, pcm_data.size());

        int16_t min_sample = INT16_MAX;
        int16_t max_sample = INT16_MIN;

        for (int i = start; i < end; ++i) {
            min_sample = qMin(min_sample, pcm_data[i]);
            max_sample = qMax(max_sample, pcm_data[i]);
        }

        int y1 = mid_y - (max_sample * mid_y) / std::numeric_limits<int16_t>::max();
        int y2 = mid_y - (min_sample * mid_y) / std::numeric_limits<int16_t>::max();

        painter.drawLine(x, y1, x, y2);
    }

    return image;
}

// 按照帧步长加载缩略图
QList<QImage> TimelineMediaUtil::loadVideoThumbnails(const QString& path, int height, int frame_step)
{
    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, path.toStdString().c_str(), nullptr, nullptr) != 0 || avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        if (fmt_ctx)
            avformat_close_input(&fmt_ctx);
        return {};
    }

    // 查找视频流
    int video_stream_idx = -1;
    for (unsigned i = 0; i < fmt_ctx->nb_streams && video_stream_idx == -1; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
        }
    }

    if (video_stream_idx == -1) {
        avformat_close_input(&fmt_ctx);
        return {};
    }

    AVStream* video_stream = fmt_ctx->streams[video_stream_idx];
    AVCodecParameters* codec_par = video_stream->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    AVCodecContext* codec_ctx = codec ? avcodec_alloc_context3(codec) : nullptr;

    if (!codec_ctx || avcodec_parameters_to_context(codec_ctx, codec_par) < 0 || avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        if (codec_ctx)
            avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return {};
    }

    // 计算视频总帧数和时长
    int64_t duration = fmt_ctx->duration;
    if (duration == AV_NOPTS_VALUE) {
        duration = video_stream->duration * av_q2d(video_stream->time_base) * AV_TIME_BASE;
    }

    double fps = av_q2d(video_stream->r_frame_rate);
    int64_t total_frames = (duration * fps) / AV_TIME_BASE;

    // 计算缩放后的尺寸
    int target_width = (codec_ctx->width * height) / codec_ctx->height;

    // 初始化转换器
    SwsContext* sws_ctx = sws_getContext(
        codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, target_width, height, AV_PIX_FMT_RGB24, SWS_FAST_BILINEAR, nullptr, nullptr, nullptr);

    if (!sws_ctx) {
        avcodec_free_context(&codec_ctx);
        avformat_close_input(&fmt_ctx);
        return {};
    }

    // 分配帧和缓冲区
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgb_frame = av_frame_alloc();

    int rgb_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, target_width, height, 1);
    uint8_t* rgb_buffer = (uint8_t*)av_malloc(rgb_buffer_size);
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, rgb_buffer, AV_PIX_FMT_RGB24, target_width, height, 1);

    QList<QImage> thumbnails;

    // 使用跳跃式提取，而不是顺序遍历
    for (int64_t target_frame = 0; target_frame < total_frames; target_frame += frame_step) {
        // 计算目标时间戳
        int64_t timestamp = (target_frame * AV_TIME_BASE) / fps;

        // 跳转到目标位置
        if (av_seek_frame(fmt_ctx, -1, timestamp, AVSEEK_FLAG_BACKWARD) < 0) {
            continue;
        }

        // 清空解码器缓冲区
        avcodec_flush_buffers(codec_ctx);

        // 寻找最近的关键帧
        bool found_frame = false;
        int attempts = 0;
        while (av_read_frame(fmt_ctx, packet) >= 0 && attempts < 10) {
            if (packet->stream_index == video_stream_idx) {
                if (avcodec_send_packet(codec_ctx, packet) == 0) {
                    if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                        sws_scale(sws_ctx, frame->data, frame->linesize, 0, codec_ctx->height, rgb_frame->data, rgb_frame->linesize);

                        QImage img(rgb_frame->data[0], target_width, height, rgb_frame->linesize[0], QImage::Format_RGB888);
                        thumbnails.append(img.copy());
                        found_frame = true;
                        break;
                    }
                }
                attempts++;
            }
            av_packet_unref(packet);
        }

        if (!found_frame) {
            av_packet_unref(packet);
        }
    }

    // 清理资源
    av_free(rgb_buffer);
    av_frame_free(&rgb_frame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    sws_freeContext(sws_ctx);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);

    return thumbnails;
}

} // namespace tl