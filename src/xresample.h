#ifndef XRESAMPLE_H
#define XRESAMPLE_H

// std ͷ�ļ� ����
#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class XResample
{
public:
	XResample();
	~XResample();

	virtual bool Open(AVCodecParameters* para, bool ClearPara = false);// ��Ƶ�ز���, �Ƿ����� para , ������,��һ�������������� XAudioThread ����
	virtual void Close();                                  
	virtual int Resample(AVFrame* frame, unsigned char* data);         // �����ز������С���ͷ�frame

	int sampleRate = 0;                                                // ��Ƶ��Ϣ
	int channels = 0;

protected:
	std::mutex m_mtx;

	int output_sampleformat = 1/*AV_SAMPLE_FMT_S16*/;
	SwrContext* swrc;
};



#endif



