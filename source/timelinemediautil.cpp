#include "timelinemediautil.h"
#include "timelinedef.h"
#include <QCoreApplication>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace tl {

std::optional<TimelineMediaUtil::MediaInfo> TimelineMediaUtil::loadMedia(const QString& path)
{

    AVFormatContext* fmt_ctx = nullptr;
    if (avformat_open_input(&fmt_ctx, path.toStdString().c_str(), nullptr, nullptr) != 0) {
        TL_LOG_ERROR("Failed to open media file: {}", path.toStdString());
        return std::nullopt;
    }
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        TL_LOG_ERROR("Failed to find stream info: {}", path.toStdString());
        return std::nullopt;
    }

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

    AVCodecContext* codec_ctx = nullptr;
    codec_ctx = avcodec_alloc_context3(nullptr);
    if (avcodec_parameters_to_context(codec_ctx, fmt_ctx->streams[video_stream_idx]->codecpar) < 0) {
        TL_LOG_ERROR("Failed to copy codec parameters to context: {}", path.toStdString());
        return {};
    }

    AVCodecParameters* codec_par = fmt_ctx->streams[video_stream_idx]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codec_par);
    avcodec_open2(codec_ctx, codec, nullptr);

    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    AVFrame* rgb_frame = av_frame_alloc();

    MediaInfo info;
    info.path = path;
    info.size.setWidth(codec_ctx->width);
    info.size.setHeight(codec_ctx->height);
    SwsContext* sws_ctx = sws_getContext(info.size.width(), info.size.height(), codec_ctx->pix_fmt, info.size.width(), info.size.height(), AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    // 获取视频帧率
    info.fps = static_cast<double>(fmt_ctx->streams[video_stream_idx]->r_frame_rate.num) / fmt_ctx->streams[video_stream_idx]->r_frame_rate.den;
    // 获取视频时长(单位：ms)
    info.duration = static_cast<double>(fmt_ctx->duration * 1000.0) / AV_TIME_BASE;

    int rgb_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_RGB24, info.size.width(), info.size.height(), 1);
    uint8_t* rgb_buffer = (uint8_t*)av_malloc(rgb_buffer_size * sizeof(uint8_t));
    av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize, rgb_buffer, AV_PIX_FMT_RGB24, info.size.width(), info.size.height(), 1);

    int frame_count = 0;
    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream_idx) {
            avcodec_send_packet(codec_ctx, packet);
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                if (frame_count % 10 == 0) {
                    sws_scale(sws_ctx, frame->data, frame->linesize, 0, info.size.height(), rgb_frame->data, rgb_frame->linesize);
                    QImage img(rgb_frame->data[0], info.size.width(), info.size.height(), rgb_frame->linesize[0], QImage::Format_RGB888);
                    info.images.append(img.copy());
                }
                frame_count++;
            }
        }
        av_packet_unref(packet);
    }

    // 释放资源
    av_frame_free(&frame);
    av_frame_free(&rgb_frame);
    av_packet_free(&packet);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    av_free(rgb_buffer);
    sws_freeContext(sws_ctx);

    return info;
}

QString TimelineMediaUtil::mediaInfoString(const MediaInfo& info)
{
    return QCoreApplication::translate("TimelineMediaUtil", "Path: %1\nSize: %2x%3\nFPS: %4\nDuration: %5ms\nImages: %6")
        .arg(info.path.isEmpty() ? "N/A" : info.path)
        .arg(info.size.width())
        .arg(info.size.height())
        .arg(info.fps)
        .arg(info.duration)
        .arg(info.images.size());
}

} // namespace tl