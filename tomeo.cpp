//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//
#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <string>
#include <vector>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include "the_player.h"
#include "the_button.h"

<<<<<<< HEAD
// 读取视频和缩略图
std::vector<TheButtonInfo> getInfoIn(std::string loc) {
    std::vector<TheButtonInfo> out;
=======
// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {
    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
>>>>>>> 81b2cf24fedab7b8f25553119737118842a9d46e
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) {
        QString f = it.next();

        if (f.contains(".")) {
#if defined(_WIN32)
            if (f.contains(".wmv")) {  // windows
#else
            if (f.contains(".mp4") || f.contains("MOV")) {  // mac/linux
#endif
                QString thumb = f.left(f.length() - 4) + ".png";
                if (QFile(thumb).exists()) {  // 如果png缩略图存在
                    QImageReader* imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read();  // 读取缩略图
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite));  // 创建按钮图标
                        QUrl* url = new QUrl(QUrl::fromLocalFile(f));  // 将文件路径转为URL
                        out.push_back(TheButtonInfo(url, ico));  // 添加到输出列表
                    }
                }
            }
        }
    }
    return out;
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    std::vector<TheButtonInfo> videos;
    if (argc == 2)
        videos = getInfoIn(std::string(argv[1]));

    if (videos.size() == 0) {
        const int result = QMessageBox::information(
            NULL,
            QString("Tomeo"),
            QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    QVideoWidget* videoWidget = new QVideoWidget;
    QSlider* slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 100);

    QPushButton* playPauseButton = new QPushButton("Play/Pause");

    // Create Like, Favorite, and Reward buttons
    QPushButton *likeButton = new QPushButton("❤️ Like");
    QPushButton *favoriteButton = new QPushButton("⭐ Favorite");
    QPushButton *rewardButton = new QPushButton("💰 Reward");

    // 点赞功能
    QObject::connect(likeButton, &QPushButton::clicked, [&]() {
        static int likeCount = 0;
        likeCount++;
        QMessageBox::information(nullptr, "点赞成功",
                                 QString("当前点赞数：%1").arg(likeCount));
    });

    // 收藏功能
    QObject::connect(favoriteButton, &QPushButton::clicked, [&]() {
        QMessageBox::information(nullptr, "Favorite Success", "Video added to your favorites!");
    });

    // Reward button functionality
    QObject::connect(rewardButton, &QPushButton::clicked, [&]() {
        QMessageBox::information(nullptr, "Reward Success", "Thank you for your support!");
    });


    actionButtonsLayout->addWidget(likeButton);
    actionButtonsLayout->addWidget(favoriteButton);
    actionButtonsLayout->addWidget(rewardButton);

    ThePlayer* player = new ThePlayer(slider);
    player->setVideoOutput(videoWidget);

    QObject::connect(playPauseButton, &QPushButton::clicked, player, &ThePlayer::togglePlayPause);

    QWidget* buttonWidget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    std::vector<TheButton*> buttons;
    for (int i = 0; i < 4; i++) {
        TheButton* button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    player->setContent(&buttons, &videos);

    QWidget window;
    QVBoxLayout* top = new QVBoxLayout();
    window.setLayout(top);
    window.setWindowTitle("Tomeo");
    window.setMinimumSize(800, 680);

    // Connect slider and player signals
    QObject::connect(slider, &QSlider::valueChanged, player, &ThePlayer::onSliderValueChanged);
    QObject::connect(player, &ThePlayer::updateSliderPosition, slider, &QSlider::setValue);

    top->addWidget(videoWidget);
    top->addWidget(playPauseButton);
    top->addWidget(slider);
    top->addLayout(actionButtonsLayout);  // 添加点赞、收藏和赞赏按钮
    top->addWidget(buttonWidget);

    window.show();

    return app.exec();
}

