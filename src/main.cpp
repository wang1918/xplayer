#include "xplayer.h"
#include <QtWidgets/QApplication>

//// std 头文件 引入
//#include <qthread.h>
//#include <qdebug.h>
//// 本地 头文件 引入
//#include "xdemux.h"
//#include "xdecode.h"
//#include "xresample.h"
//#include "xaudioplay.h"
//#include "xaudiothread.h"
//#include "xvideothread.h"
//#include "xdemuxthread.h"

//struct AVCodecParameters;
//
//
//class WorkThread : public QThread {
//public:
//    void init() {
//        //qDebug() << "m_video" << m_video << endl; 
//        /// 单步测试代码正确性
//        // 测试 XDemux
//        const char* local_url = "..\\..\\..\\data\\trailer2.mov";
//        const char* rstp_url = "https://v3.dious.cc/20220517/mpnXmL6T/index.m3u8";  // 哆啦A梦
//        //demux.Open(local_url);
//        demux.Open(local_url);
//        qDebug() << "copy video parameters : " << demux.CpVPara() << endl;
//        qDebug() << "copy audio parameters : " << demux.CpAPara() << endl;
//        //qDebug() << "seek : " << demux.Seek(0.9) << endl;
//
//        // 测试 XDecode
//        //qDebug() << "decode for video : " << vdecode.Open(demux.CpVPara());
//        //qDebug() << "decode for audio : " << adecode.Open(demux.CpAPara());
//
//        // 测试 XResample
//        //qDebug() << "resample for audio : " << resample.Open(demux.CpAPara());
//
//        // 测试 XAudioPlay
//        //XAudioPlay::Get()->channels = demux.channels;
//        //XAudioPlay::Get()->sampleRate = demux.sampleRate;
//        //qDebug() << "xaudioplay open : " << XAudioPlay::Get()->Open() << endl;
//        
//        // 测试 XAudioThread
//        AVCodecParameters* vpara = demux.CpVPara();
//        AVCodecParameters* apara = demux.CpAPara();
//        
//        //videothread.getFPS(demux.video_fps);
//        //videothread.Open(vpara, m_video, w, h);
//        //videothread.start();
//        //audiothread.Open(apara);
//        //audiothread.start();
//    }
//    //unsigned char* pcm = new unsigned char[1024 * 1024];
//    void run() override {
//        for (;;) {
//            AVPacket* pkt = demux.Read();
//            if (!pkt) {
//                return;
//            }
//            if (demux.IsAudio(pkt)) {
//                audiothread.Push(pkt);
//                //qDebug() << "audio Send : " << adecode.Send(pkt);
//                //qDebug() << "audio Recv : " << adecode.Recv();
//
//                //adecode.Send(pkt);
//                //AVFrame* frame = adecode.Recv();
//                //int len = resample.Resample(frame, pcm);                            // 重采样空间大小
//                //while (len > 0) {
//                //    if (XAudioPlay::Get()->GetFree() >= len) {
//                //        XAudioPlay::Get()->Write(pcm, len);
//                //        break;
//                //    }
//                //    msleep(1);
//                //}
//
//                //qDebug() << "resample : " << resample.Resample(frame, pcm);
//            }
//            else {
//                videothread.Push(pkt);
//                //qDebug() << "video Send : " << vdecode.Send(pkt);
//                //qDebug() << "video Recv : " << vdecode.Recv();
//                //vdecode.Send(pkt);
//                //AVFrame* frame = vdecode.Recv();
//                //m_video->rePaint(frame);   
//                //msleep(1000/demux.video_fps);
//            }
//        }
//    }
//
//    XVideoWidget* m_video = nullptr;
//
//private:
//    XDemux demux;
//    //XDecode vdecode;
//    //XDecode adecode;
//    //XResample resample;
//    XAudioThread audiothread;
//    XVideoThread videothread;
//};
////WorkThread wt;
////wt.m_video = w.getVideoWidget();
////wt.init();
////wt.start();

/// 主函数入口

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    xplayer w;
    w.show();
    
    //const char* local_url = "..\\..\\..\\data\\trailer2.mov";
    //const char* rstp_url = "https://v3.dious.cc/20220517/mpnXmL6T/index.m3u8";  // 哆啦A梦
    //XDemuxThread dt;
    //dt.Open(local_url, w.getVideoWidget());
    //dt.Start();
    
    return a.exec();
}
