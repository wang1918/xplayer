#ifndef XAUDIOTHREAD_H
#define XAUDIOTHREAD_H

// QT ͷ�ļ�����
#include <qthread.h>
// STD ͷ�ļ�����
#include <mutex>
#include <queue>
// �����ļ�����
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

	virtual bool Open(AVCodecParameters* para, int sampleRate, int channels);// ���� para ���ڴ�
                                           
	void run();

	virtual void Stop(bool isPause);

	virtual void Close();

	long long pts = 0;                                                       // ������Ƶͬ���õ� pts

protected:
	bool isPause = false;

	XAudioPlay* audioplay = nullptr;
	XResample* resample = nullptr;
	std::mutex m_a_mtx;
};

#endif 



