#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_xplayer.h"

class XDemuxThread;

class xplayer : public QMainWindow
{
    Q_OBJECT

public:
    xplayer(QWidget *parent = nullptr);
    ~xplayer();

    // 定时器 滑动条显示
    void timerEvent(QTimerEvent* event);
    // 窗口尺寸变化
    void resizeEvent(QResizeEvent* event);
    // 双击全屏 双击窗口边上
    void mouseDoubleClickEvent(QMouseEvent* event);

private slots:
    void OpenFile();
    void Play();

private:
    bool fristOpen = true;
    bool isplaying;

    QString url;

    XDemuxThread* demuxthread = nullptr;

private:
    Ui::xplayerClass ui;
};
