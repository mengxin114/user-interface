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

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {
    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) {
        QString f = it.next();

        if (f.contains("."))
#if defined(_WIN32)
        if (f.contains(".wmv")) {
#else
        if (f.contains(".mp4") || f.contains("MOV")) {
#endif
            QString thumb = f.left(f.length() - 4) + ".png";
            if (QFile(thumb).exists()) {
                QImageReader *imageReader = new QImageReader(thumb);
                QImage sprite = imageReader->read();
                if (!sprite.isNull()) {
                    QIcon *ico = new QIcon(QPixmap::fromImage(sprite));
                    QUrl *url = new QUrl(QUrl::fromLocalFile(f));
                    out.push_back(TheButtonInfo(url, ico));
                } else {
                    qDebug() << "Warning: skipping video because I couldn't process thumbnail " << thumb;
                }
            } else {
                qDebug() << "Warning: skipping video because I couldn't find thumbnail " << thumb;
            }
        }
    }

    return out;
}

int main(int argc, char *argv[]) {

    // Let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR;

    // Create the Qt Application
    QApplication app(argc, argv);

    // Collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn(std::string(argv[1]));

    if (videos.size() == 0) {
        QMessageBox::information(NULL, QString("Tomeo"), QString("No videos found! Please add command line argument for file location."));
        exit(-1);
    }

    // The widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // Create the QSlider (progress bar)
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setStyleSheet(
        "QSlider {"
        "   height: 12px;"
        "   background: #444;"
        "   border-radius: 6px;"
        "}"
        "QSlider::groove:horizontal {"
        "   height: 8px;"
        "   background: #666;"
        "   border-radius: 4px;"
        "}"
        "QSlider::handle:horizontal {"
        "   background: #0078d7;"
        "   width: 15px;"
        "   border-radius: 7px;"
        "   border: 2px solid #fff;"
        "}"
        "QSlider::handle:horizontal:hover {"
        "   background: #0056a1;"
        "   border-color: #ccc;"
        "}"
    );
    slider->setRange(0, 100);
    slider->setValue(0);

    // Create Like, Favorite, and Reward buttons
    QPushButton *likeButton = new QPushButton("❤️ Like");
    QPushButton *favoriteButton = new QPushButton("⭐ Favorite");
    QPushButton *rewardButton = new QPushButton("💰 Reward");

    // Like button functionality
    QObject::connect(likeButton, &QPushButton::clicked, [&]() {
        static int likeCount = 0;
        likeCount++;
        QMessageBox::information(nullptr, "Like Success", QString("Current Like Count: %1").arg(likeCount));
    });

    // Favorite button functionality
    QObject::connect(favoriteButton, &QPushButton::clicked, [&]() {
        QMessageBox::information(nullptr, "Favorite Success", "Video added to your favorites!");
    });

    // Reward button functionality
    QObject::connect(rewardButton, &QPushButton::clicked, [&]() {
        QMessageBox::information(nullptr, "Reward Success", "Thank you for your support!");
    });

    // Set button styles
    QString buttonStyle = "QPushButton {"
        "font-size: 18px;"
        "color: white;"
        "border: none;"
        "padding: 12px 25px;"
        "border-radius: 15px;"
        "background-color: #5c9f5f;"
        "box-shadow: 0px 4px 10px rgba(0, 0, 0, 0.2);"
    "}"
    "QPushButton:hover {"
        "background-color: #4a7d4a;"
        "box-shadow: 0px 6px 12px rgba(0, 0, 0, 0.3);"
    "}"
    "QPushButton:pressed {"
        "background-color: #3c5e3c;"
        "box-shadow: 0px 2px 5px rgba(0, 0, 0, 0.1);"
    "}";

    likeButton->setStyleSheet(buttonStyle);
    favoriteButton->setStyleSheet(buttonStyle);
    rewardButton->setStyleSheet(buttonStyle);

    // Create action buttons layout
    QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
    actionButtonsLayout->addWidget(likeButton);
    actionButtonsLayout->addWidget(favoriteButton);
    actionButtonsLayout->addWidget(rewardButton);

    // The QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer(slider);
    player->setVideoOutput(videoWidget);

    // Pause and play button
    QPushButton *playPauseButton = new QPushButton("Play/Pause");
    QObject::connect(playPauseButton, &QPushButton::clicked, player, &ThePlayer::togglePlayPause);
    playPauseButton->setStyleSheet(
        "QPushButton {"
        "background-color: #4CAF50;"
        "color: white;"
        "border: none;"
        "padding: 15px 32px;"
        "text-align: center;"
        "font-size: 16px;"
        "border-radius: 8px;"
        "} "
        "QPushButton:hover {"
        "background-color: #45a049;"
        "}"
    );

    // Create button widget for video selection
    QWidget *buttonWidget = new QWidget();
    std::vector<TheButton*> buttons;
    QHBoxLayout *layout = new QHBoxLayout();
    buttonWidget->setLayout(layout);

    // Create video selection buttons
    for (int i = 0; i < 4; i++) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo*)), player, SLOT(jumpTo(TheButtonInfo*)));
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // Set the player content with buttons and videos
    player->setContent(&buttons, &videos);

    // Create main window layout
    QWidget window;
    QVBoxLayout *topLayout = new QVBoxLayout();
    topLayout->setContentsMargins(10, 10, 10, 10);
    topLayout->setSpacing(15);  // Control spacing between widgets
    window.setLayout(topLayout);
    window.setWindowTitle("Tomeo - Sports Video Player");
    window.setMinimumSize(900, 700);

    // Connect slider and player signals
    QObject::connect(slider, &QSlider::valueChanged, player, &ThePlayer::onSliderValueChanged);
    QObject::connect(player, &ThePlayer::updateSliderPosition, slider, &QSlider::setValue);

    // Add video, buttons, and action buttons to layout
    topLayout->addWidget(videoWidget);
    topLayout->addWidget(playPauseButton);
    topLayout->addWidget(slider);
    topLayout->addLayout(actionButtonsLayout); // Add Like, Favorite, and Reward buttons
    topLayout->addWidget(buttonWidget);

    // Show the window
    window.show();

    // Wait for the app to terminate
    return app.exec();
}


