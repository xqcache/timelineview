#include "playbackvideoplayer.h"
#include <QApplication>
#include <QFileDialog>
#include <QLabel>
#include <QTimer>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // QString video_path = QFileDialog::getOpenFileName();
    QString video_path = R"(C:\Users\xqliang\Desktop\video1.mp4)";
    if (video_path.isEmpty()) {
        return -1;
    }

    PlaybackVideoPlayer player;
    if (!player.open(video_path)) {
        return -2;
    }

    QLabel label;
    label.resize(player.size());
    label.show();

    player.play(2);

    QTimer timer;
    timer.setInterval(1);
    timer.callOnTimeout([&label, &player] {
        auto elapsed_st = std::chrono::steady_clock::now();
        label.setPixmap(QPixmap::fromImage(player.getImage()));
        // qDebug() << "==painter elapsed: " << (std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - elapsed_st).count());
    });
    timer.start();

    return app.exec();
}