#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

// QT 头文件引入
#include <qthread.h>
// STD 头文件引入
#include <mutex>
#include <queue>

class XDecode;
class XAudioPlay;
class XResample;
struct AVCodecParameters;
struct AVPacket;

class XDecodeThread : public QThread
{
public:
	XDecodeThread();
	virtual ~XDecodeThread();

	virtual void Push(AVPacket* pkt);
	virtual AVPacket* Pop();                              // 取出一帧数据并出栈，没有取到返回 nullptr
	virtual void Clear();
	virtual void Close();

	// 最大队列
	int maxSize = 100;
	bool isExit = false;

protected:
	std::queue<AVPacket*> m_pkt_que;
	std::mutex m_mtx;
	XDecode* decode = nullptr;
};


#endif // !XDECODETHREAD_H
