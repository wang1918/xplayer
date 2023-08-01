#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

// QT 头文件引入
#include <qthread.h>
// STD 头文件引入
#include <mutex>
// 本地文件引入
#include "ivideocall.h"

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread : public QThread
{
public:
	XDemuxThread();
	virtual ~XDemuxThread();

	virtual bool Open(const char* url, IVideoCall* call);         // Open XVideoThread 和 XAudioThread
	virtual void Start();                                         // start videothread 和 audiothread
	virtual void Close();

	virtual void Stop(bool isPause);

	void run();

	long long pts = 0;                                            // 便于滑动条滑动
	long long totalMs = 0;
	
private:
	std::mutex m_mtx;
	bool isExit = false;
	bool isPause = false;      // 暂停

	IVideoCall* videocall = nullptr;
	XDemux* demux = nullptr;
	XVideoThread* videothread = nullptr;
	XAudioThread* audiothread = nullptr;
};


#endif // !XDEMUXTHREAD_H



