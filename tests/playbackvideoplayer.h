#pragma once

#include <QImage>
#include <QString>
#include <stop_token>

struct PlaybackVideoPlayerPrivate;

class PlaybackVideoPlayer {
public:
    PlaybackVideoPlayer();
    ~PlaybackVideoPlayer() noexcept;

    bool open(const QString& video_path);
    void close();

    QSize size() const;

    bool play(qint64 frame_step = 1);
    void pause();
    void stop();

    // 获取当前帧图像
    QImage getImage() const;

    // 直接跳转到指定帧
    bool seekToFrame(qint64 frame_no);

    // 获取播放状态
    bool isPlaying() const;
    qint64 getCurrentFrame() const;
    qint64 getTotalFrames() const;

    // 预加载帧以减少闪烁
    void preloadFrame(qint64 frame_no);

private:
    void run(std::stop_token st);

    QImage decodeFrame(qint64 frame_no) const;

    // 帧缓存管理
    bool hasValidFrame() const;
    void clearFrameCache();

private:
    PlaybackVideoPlayerPrivate* d_ { nullptr };
};
