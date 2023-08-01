#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

// QT 头文件引入
#include <qthread.h>
// STD 头文件引入
#include <mutex>
#include <queue>
// 本地 头文件引入
#include "xdecodethread.h"

class XDecode;
class XVideoWidget;
class IVideoCall;

struct AVCodecParameters;
struct AVPacket;


class XVideoThread : public XDecodeThread
{
public:
	XVideoThread();
	virtual ~XVideoThread();

	virtual bool Open(AVCodecParameters* para, IVideoCall* videocall, int width, int height);               // 清理 para 的内存
	void run();
	virtual void Stop(bool isPause);

	long long synpts = 0;                                                                                   // 同步时间，由外部传入

protected:
	bool isPause = false;
	std::mutex m_v_mtx;
	IVideoCall* call = nullptr;
};

#endif // !XVIDEOTHREAD_H



