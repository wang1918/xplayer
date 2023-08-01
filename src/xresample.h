#ifndef XRESAMPLE_H
#define XRESAMPLE_H

// std 头文件 引入
#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class XResample
{
public:
	XResample();
	~XResample();

	virtual bool Open(AVCodecParameters* para, bool ClearPara = false);// 音频重采样, 是否清理 para , 不清理,这一步交给控制总类 XAudioThread 来做
	virtual void Close();                                  
	virtual int Resample(AVFrame* frame, unsigned char* data);         // 返回重采样后大小，释放frame

	int sampleRate = 0;                                                // 音频信息
	int channels = 0;

protected:
	std::mutex m_mtx;

	int output_sampleformat = 1/*AV_SAMPLE_FMT_S16*/;
	SwrContext* swrc;
};



#endif



