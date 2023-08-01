#include "xplayer.h"
// QT ͷ�ļ�����
#include <qaction.h>
#include <qfiledialog.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qmessagebox.h>
// �����ļ�����
#include "xdemuxthread.h"

xplayer::xplayer(QWidget *parent)
    : QMainWindow(parent)
{
    
    ui.setupUi(this);

    if (!demuxthread) {
        demuxthread = new XDemuxThread();
    }
    

    connect(ui.act_openfile, &QAction::triggered, this, &xplayer::OpenFile);
    connect(ui.pb_play, &QPushButton::clicked, this, &xplayer::Play);

    startTimer(40);
}

xplayer::~xplayer()
{
    demuxthread->Close();
}

void xplayer::OpenFile() {
    url = QFileDialog::getOpenFileName(this, QStringLiteral("ѡ������Ƶ�ļ�"), "..\\..\\..\\data\\", QStringLiteral("��Ƶ�ļ�(*mp4 *mov);;""��Ƶ�ļ�(*mp3);;"));
    if (url.isEmpty()) {
        return;
    }
    this->setWindowTitle(url);
    if (!demuxthread->Open(url.toStdString().c_str(), ui.widget)) {
        QMessageBox::information(0, "open", "open failed!");
    }
    isplaying = false;
}

void xplayer::Play() {
    if (fristOpen) {
        demuxthread->Start();
        fristOpen = false;
        isplaying = true;
    }
    else {
        if (isplaying) {
            demuxthread->Stop(true);   // ��ͣ��
            isplaying = false;
        }
        else {
            demuxthread->Stop(false);  // ��������
            isplaying = true;
        }
    }
}

void xplayer::timerEvent(QTimerEvent* event) {
    long long total = demuxthread->totalMs;
    if (total > 0) {
        double pos = demuxthread->pts / (double)total;
        int v = ui.video_slider->maximum()* pos;
        ui.video_slider->setValue(v);
    }
}

void xplayer::resizeEvent(QResizeEvent* event) {
    ui.widget->resize(this->size());
}

void xplayer::mouseDoubleClickEvent(QMouseEvent* event) {
    if (isFullScreen()) {
        this->showFullScreen();
    }
    else {
        this->showNormal();
    }
}