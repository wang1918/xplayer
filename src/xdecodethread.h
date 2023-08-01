#ifndef XDECODETHREAD_H
#define XDECODETHREAD_H

// QT ͷ�ļ�����
#include <qthread.h>
// STD ͷ�ļ�����
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
	virtual AVPacket* Pop();                              // ȡ��һ֡���ݲ���ջ��û��ȡ������ nullptr
	virtual void Clear();
	virtual void Close();

	// ������
	int maxSize = 100;
	bool isExit = false;

protected:
	std::queue<AVPacket*> m_pkt_que;
	std::mutex m_mtx;
	XDecode* decode = nullptr;
};


#endif // !XDECODETHREAD_H
