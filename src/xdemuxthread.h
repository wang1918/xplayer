#ifndef XDEMUXTHREAD_H
#define XDEMUXTHREAD_H

// QT ͷ�ļ�����
#include <qthread.h>
// STD ͷ�ļ�����
#include <mutex>
// �����ļ�����
#include "ivideocall.h"

class XDemux;
class XVideoThread;
class XAudioThread;

class XDemuxThread : public QThread
{
public:
	XDemuxThread();
	virtual ~XDemuxThread();

	virtual bool Open(const char* url, IVideoCall* call);         // Open XVideoThread �� XAudioThread
	virtual void Start();                                         // start videothread �� audiothread
	virtual void Close();

	virtual void Stop(bool isPause);

	void run();

	long long pts = 0;                                            // ���ڻ���������
	long long totalMs = 0;
	
private:
	std::mutex m_mtx;
	bool isExit = false;
	bool isPause = false;      // ��ͣ

	IVideoCall* videocall = nullptr;
	XDemux* demux = nullptr;
	XVideoThread* videothread = nullptr;
	XAudioThread* audiothread = nullptr;
};


#endif // !XDEMUXTHREAD_H



