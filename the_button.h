//
//
//

#ifndef CW2_THE_BUTTON_H
#define CW2_THE_BUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QToolButton>

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
    VolumeIconButton(QWidget* parent = nullptr) : QToolButton(parent) {
        setIcon(QIcon(":/icons/volume-high.png")); // 设置默认音量图标
        setStyleSheet(
            "QToolButton {"
            "   background: transparent;"
            "   border: none;"
            "}"
            "QToolButton:hover {"
            "   background: #444;"
            "}"
        );
    }

    void setMuteIcon() {
        setIcon(QIcon(":/icons/volume-mute.png")); // 设置静音图标
    }

    void setHighVolumeIcon() {
        setIcon(QIcon(":/icons/volume-high.png")); // 设置高音量图标
    }

signals:
    void muteToggled(bool isMuted); // 信号，用于通知音量状态变化
};
#endif //CW2_THE_BUTTON_H
