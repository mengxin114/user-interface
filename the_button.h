//
//
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QToolButton>
#include <QIcon>
#include <QString>
#include <QDateTime>
#include <QToolTip>
#include <QPixmap>
#include <QPainter>
#include <QColor>

class TheButtonInfo {

public:
    QUrl* url; // video file to play
    QIcon* icon; // icon to display

    TheButtonInfo ( QUrl* url, QIcon* icon) : url (url), icon (icon) {}
};

class TheButton : public QPushButton {
    Q_OBJECT

public:
    TheButtonInfo* info;

     TheButton(QWidget *parent) :  QPushButton(parent) {
         setIconSize(QSize(200,110));
         connect(this, SIGNAL(released()), this, SLOT (clicked() )); // if QPushButton clicked...then run clicked() below
    }

    void init(TheButtonInfo* i);

private slots:
    void clicked();

signals:
    void jumpTo(TheButtonInfo*);

};

// 新增 VolumeIconButton 类
class VolumeIconButton : public QToolButton {
    Q_OBJECT
public:
    explicit VolumeIconButton(QWidget* parent = nullptr) : QToolButton(parent), isMuted(true) {
        // 初始化为静音图标（默认音量为零）
        setMuteIcon();
        setIconSize(QSize(40, 40));

        // 设置按钮样式
        setStyleSheet(
            "QToolButton {"
            "   background: rgba(255, 255, 255, 0.3);"
            "   border: none;"
            "   padding: 5px;"
            "} "
            "QToolButton:hover {"
            "   background: rgba(255, 255, 255, 0.5);"
            "} "
        );

        // 连接按钮点击事件到静音切换槽
        connect(this, &QToolButton::clicked, this, &VolumeIconButton::toggleMute);

        // 添加额外的信号槽，用于同步静音状态到播放器
        connect(this, &VolumeIconButton::muteToggled, this, [this](bool isMuted) {
            if (isMuted) {
                setMuteIcon(); // 设置静音图标
            } else {
                setHighVolumeIcon(); // 设置高音量图标
            }
        });
    }

    // 设置静音图标
    void setMuteIcon() {
        setIcon(QIcon(":/icon/a/no_voice.png"));
    }

    // 设置高音量图标
    void setHighVolumeIcon() {
        setIcon(QIcon(":/icon/a/voice.png"));
    }

    // 切换静音状态
    void toggleMute() {
        isMuted = !isMuted; // 切换静音状态
        emit muteToggled(isMuted); // 发射静音信号
    }

signals:
    void muteToggled(bool isMuted); // 信号，用于通知静音状态变化

private:
    bool isMuted; // 记录当前是否静音
};

#endif //CW2_THE_BUTTON_H
