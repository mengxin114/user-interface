//
//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include "the_button.h"
#include "the_player.h"
#include <QApplication>
#include <QDesktopServices>
#include <QImageReader>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QtWidgets/QFileIconProvider>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
#include <iostream>
#include <string>
#include <vector>

// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn(std::string loc) {

  std::vector<TheButtonInfo> out = std::vector<TheButtonInfo>();
  QDir dir(QString::fromStdString(loc));
  QDirIterator it(dir);

  while (it.hasNext()) { // for all files

    QString f = it.next();

    if (f.contains("."))

#if defined(_WIN32)
      if (f.contains(".wmv")) { // windows
#else
      if (f.contains(".mp4") || f.contains("MOV")) { // mac/linux
#endif

        QString thumb = f.left(f.length() - 4) + ".png";
        if (QFile(thumb).exists()) { // if a png thumbnail exists
          QImageReader *imageReader = new QImageReader(thumb);
          QImage sprite = imageReader->read(); // read the thumbnail
          if (!sprite.isNull()) {
            QIcon *ico = new QIcon(QPixmap::fromImage(
                sprite)); // voodoo to create an icon for the button
            QUrl *url = new QUrl(QUrl::fromLocalFile(
                f)); // convert the file location to a generic url
            out.push_back(TheButtonInfo(url, ico)); // add to the output list
          } else
            qDebug() << "warning: skipping video because I couldn't process "
                        "thumbnail "
                     << thumb << endl;
        } else
          qDebug()
              << "warning: skipping video because I couldn't find thumbnail "
              << thumb << endl;
      }
  }

  return out;
}

class ResponsiveWindow : public QWidget {
  Q_OBJECT

public:
  ResponsiveWindow(QWidget *buttonWidget, QVBoxLayout *mainContentLayout)
      : buttonWidget(buttonWidget), mainContentLayout(mainContentLayout) {
    setWindowTitle("Tomeo");
    setMinimumSize(800, 600);
    setStyleSheet("background-color: black; color: white;");
  }

protected:
  void resizeEvent(QResizeEvent *event) override {
    QWidget::resizeEvent(event);

    // 判断窗口宽高比例，当宽度小于高度时隐藏 Sidebar
    if (width() < height() * 0.8) { // 窗口接近手机比例时
      buttonWidget->hide();         // 隐藏 Sidebar
    } else {
      buttonWidget->show(); // 显示 Sidebar
    }
  }

private:
  QWidget *buttonWidget;          // Sidebar 控件
  QVBoxLayout *mainContentLayout; // 主内容区域布局
};

int main(int argc, char *argv[]) {

  // let's just check that Qt is operational first
  qDebug() << "Qt version: " << QT_VERSION_STR << endl;

  // create the Qt Application
  QApplication app(argc, argv);

  // collect all the videos in the folder
  std::vector<TheButtonInfo> videos;

  if (argc == 2)
    videos = getInfoIn(std::string(argv[1]));

  if (videos.size() == 0) {

    const int result = QMessageBox::information(
        NULL, QString("Tomeo"),
        QString("no videos found! Add command line argument to \"quoted\" file "
                "location."));
    exit(-1);
  }

  // the widget that will show the video
  QVideoWidget *videoWidget = new QVideoWidget;
  videoWidget->setStyleSheet("QVideoWidget {"
                             "   background-color: black;" // 播放区域背景色
                             "   border: 2px solid #444444;"        // 边框
                             "   border-radius: 10px;"              // 圆角
                             "   margin: 10px;"                     // 外边距
                             "   box-shadow: 0px 0px 10px #000000;" // 阴影效果
                             "}");

  // create the QSlider
  QSlider *slider = new QSlider(Qt::Horizontal);
  // 设置滑块的样式表
  slider->setStyleSheet("QSlider {"
                        "   height: 12px;"   // 整个进度条的高度
                        "   margin: 0 12px;" // 左右边距
                        "}"
                        "QSlider::groove:horizontal {"
                        "   background: #555555;" // 滑动槽的背景颜色
                        "   height: 12px;"        // 滑动槽的高度
                        "   border-radius: 6px;"  // 滑动槽的圆角
                        "   border: 1px solid #444444;" // 滑动槽的边框
                        "}"
                        "QSlider::handle:horizontal {"
                        "   background: #00A8FF;" // 滑块的背景颜色（亮蓝色）
                        "   width: 20px;"               // 滑块的宽度
                        "   height: 20px;"              // 滑块的高度
                        "   border-radius: 10px;"       // 滑块为圆形
                        "   border: 2px solid #0078D7;" // 滑块边框颜色
                        "   margin: -5px 0;" // 调整滑块在槽中的位置
                        "}"
                        "QSlider::handle:horizontal:hover {"
                        "   background: #0056A1;" // 悬停时滑块的背景颜色
                        "   border: 2px solid #003E7E;" // 悬停时边框颜色
                        "}"
                        "QSlider::sub-page:horizontal {"
                        "   background: #00A8FF;" // 滑动槽已填充部分的颜色
                        "   border-radius: 6px;" // 已填充部分的圆角
                        "}"
                        "QSlider::add-page:horizontal {"
                        "   background: #555555;" // 滑动槽未填充部分的颜色
                        "   border-radius: 6px;" // 未填充部分的圆角
                        "}");

  slider->setRange(0, 100);
  slider->setValue(0);

  // 创建音量滑块
  QSlider *volumeSlider = new QSlider(Qt::Vertical);
  volumeSlider->setRange(0, 100);
  volumeSlider->setValue(0);
  // 设置音量滑块的样式表
  volumeSlider->setStyleSheet("QSlider {"
                              "   width: 30px;"
                              "   background: transparent;"
                              "}"
                              "QSlider::groove:vertical {"
                              "   width: 5px;"
                              "   background: #444;"
                              "   border-radius: 2px;"
                              "}"
                              "QSlider::handle:vertical {"
                              "   height: 15px;"
                              "   width: 15px;"
                              "   background: #0078d7;"
                              "   border-radius: 7px;"
                              "   margin: 0 -5px;"
                              "}"
                              "QSlider::handle:vertical:hover {"
                              "   background: #0056a1;"
                              "}"
                              "QSlider::add-page:vertical {"
                              "   background: #0078d7;"
                              "   border-radius: 2px;"
                              "}"
                              "QSlider::sub-page:vertical {"
                              "   background: #666;"
                              "   border-radius: 2px;"
                              "}");

  // 创建音量图标按钮
  VolumeIconButton *volumeIcon = new VolumeIconButton;

  // 创建一个水平布局，用于放置视频控件和音量滑块
  QHBoxLayout *videoAndVolumeLayout = new QHBoxLayout();
  videoAndVolumeLayout->addWidget(videoWidget); // 添加视频控件
  videoAndVolumeLayout->addSpacing(20); // 增加视频控件和音量滑块之间的间距
  videoAndVolumeLayout->addWidget(volumeSlider); // 添加音量滑块
  videoAndVolumeLayout->addSpacing(20); // 增加音量滑块和窗口边缘的间距

  // 创建点赞、收藏和赞赏按钮
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
    QMessageBox::information(nullptr, "收藏成功", "已将该视频加入收藏列表！");
  });

  // 赞赏功能
  QObject::connect(rewardButton, &QPushButton::clicked, [&]() {
    QMessageBox::information(nullptr, "赞赏成功", "感谢您的支持！");
  });

  // 设置按钮样式
  likeButton->setStyleSheet(
      "QPushButton { font-size: 16px; color: white; background-color: #f66; "
      "border-radius: 5px; padding: 10px; }");
  favoriteButton->setStyleSheet(
      "QPushButton { font-size: 16px; color: white; background-color: #fa0; "
      "border-radius: 5px; padding: 10px; }");
  rewardButton->setStyleSheet(
      "QPushButton { font-size: 16px; color: white; background-color: #3a3; "
      "border-radius: 5px; padding: 10px; }");

  // 创建一个水平布局，用于三个按钮
  QHBoxLayout *actionButtonsLayout = new QHBoxLayout();
  actionButtonsLayout->addWidget(likeButton);
  actionButtonsLayout->addWidget(favoriteButton);
  actionButtonsLayout->addWidget(rewardButton);

  // the QMediaPlayer which controls the playback
  ThePlayer *player = new ThePlayer(slider, volumeSlider);
  player->setVideoOutput(videoWidget);

  // pause and play button
  QPushButton *playPauseButton = new QPushButton("Play/Pause");
  QObject::connect(playPauseButton, &QPushButton::clicked, player,
                   &ThePlayer::togglePlayPause);
  playPauseButton->setStyleSheet("QPushButton {"
                                 "   background-color: #4CAF50;"
                                 "   color: white;"
                                 "   border: none;"
                                 "   padding: 10px 20px;"
                                 "   border-radius: 8px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "   background-color: #45a049;"
                                 "}");
  // a row of buttons
  QWidget *buttonWidget = new QWidget();
  buttonWidget->setStyleSheet("QWidget {"
                              "   background-color: #222222;" // 按钮区域背景色
                              "   border: 2px solid #333333;" // 边框
                              "   border-radius: 10px;"       // 圆角
                              "   padding: 10px;"             // 内边距
                              "   margin: 10px;"              // 外边距
                              "   box-shadow: 0px 0px 10px #111111;" // 阴影效果
                              "}");
  // a list of the buttons
  std::vector<TheButton *> buttons;
  // the buttons are arranged horizontally
  QVBoxLayout *layout = new QVBoxLayout();
  buttonWidget->setLayout(layout);

  // create the four buttons
  for (int i = 0; i < 4; i++) {
    TheButton *button = new TheButton(buttonWidget);
    button->connect(
        button, SIGNAL(jumpTo(TheButtonInfo *)), player,
        SLOT(
            jumpTo(TheButtonInfo *))); // when clicked, tell the player to play.
    buttons.push_back(button);
    layout->addWidget(button); // 将按钮添加到垂直布局中
    button->init(&videos.at(i));
    button->setStyleSheet("QPushButton {"
                          "   background-color: #333333;" // 按钮背景色
                          "   color: white;"              // 文字颜色
                          "   border: 2px solid #555555;" // 按钮边框
                          "   border-radius: 8px;"        // 按钮圆角
                          "   padding: 8px;"              // 内边距
                          "   margin-bottom: 5px;"        // 按钮间距
                          "}"
                          "QPushButton:hover {"
                          "   background-color: #444444;" // 悬停时的背景色
                          "   border-color: #0078D7;" // 悬停时边框颜色
                          "}"
                          "QPushButton:pressed {"
                          "   background-color: #0056A1;" // 按下时的背景色
                          "}");
  }

  // tell the player what buttons and videos are available
  player->setContent(&buttons, &videos);

  // create the main window and layout
  QWidget window;
  QHBoxLayout *mainLayout = new QHBoxLayout(); // 使用水平布局
  QVBoxLayout *top = new QVBoxLayout();        // 主内容区的垂直布局
  mainLayout->addWidget(buttonWidget, 1); // 左侧添加按钮区域，占1份空间
  mainLayout->addLayout(top, 3); // 右侧添加主内容区，占3份空间
  window.setLayout(mainLayout);  // 设置窗口的主布局

  window.setWindowTitle("tomeo");
  window.setMinimumSize(800, 680);
  // 设置整个应用的背景为黑色
  window.setStyleSheet("background-color: black; color: white;");

  QObject::connect(slider, &QSlider::valueChanged, player,
                   &ThePlayer::onSliderValueChanged);
  QObject::connect(player, &ThePlayer::updateSliderPosition, slider,
                   &QSlider::setValue);

  // add the video and the buttons to the top level widget
  top->addLayout(videoAndVolumeLayout);
  top->addWidget(playPauseButton);
  top->addWidget(slider);
  top->addLayout(actionButtonsLayout); // 添加点赞、收藏和赞赏按钮
  // showtime!
  window.show();

  // wait for the app to terminate
  return app.exec();
}