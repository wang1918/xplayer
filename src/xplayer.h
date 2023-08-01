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

    // ��ʱ�� ��������ʾ
    void timerEvent(QTimerEvent* event);
    // ���ڳߴ�仯
    void resizeEvent(QResizeEvent* event);
    // ˫��ȫ�� ˫�����ڱ���
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
