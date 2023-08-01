#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

// QT 头文件引入
#include <qthread.h>
// STD 头文件引入
#include <mutex>
#include <queue>
// 本地文件引入
#include "xdecodethread.h"

class XDecode;
class XAudioPlay;
class XResample;
struct AVCodecParameters;
struct AVPacket;

class XAudioThread : public XDecodeThread
{
public:
	XAudioThread();
	virtual ~XAudioThread();

	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);// 清理 para 的内存
                                           
	void run();

	virtual void Stop(bool isPause);

	virtual void Close();

	long long pts = 0;                                                       // 做音视频同步用到 pts

protected:
	bool isPause = false;

	XAudioPlay* audioplay = nullptr;
	XResample* resample = nullptr;
	std::mutex m_a_mtx;
};

#endif 



