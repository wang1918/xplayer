#ifndef XVIDEOTHREAD_H
#define XVIDEOTHREAD_H

// QT ͷ�ļ�����
#include <qthread.h>
// STD ͷ�ļ�����
#include <mutex>
#include <queue>
// ���� ͷ�ļ�����
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

	virtual bool Open(AVCodecParameters* para, IVideoCall* videocall, int width, int height);               // ���� para ���ڴ�
	void run();
	virtual void Stop(bool isPause);

	long long synpts = 0;                                                                                   // ͬ��ʱ�䣬���ⲿ����

protected:
	bool isPause = false;
	std::mutex m_v_mtx;
	IVideoCall* call = nullptr;
};

#endif // !XVIDEOTHREAD_H



