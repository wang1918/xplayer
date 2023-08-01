#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H
// QT 头文件引入
#include <qaudiooutput.h>
#include <qaudiodeviceinfo.h>
// STD 头文件引入
#include <mutex>

class XAudioPlay
{
public:
	static XAudioPlay* Get();                                   // 单例模式，若有其他的播放方式可以放在Get里面做工厂模式
	XAudioPlay();
	virtual ~XAudioPlay();

	virtual bool Open() = 0;                                    // 纯虚函数，让继承类实现，打开QT的音频播放
	virtual void Close() = 0;                                   // 纯虚函数，让继承类实现，关闭音频播放，为二次打开
	virtual bool Write(unsigned char* data, int datasize) = 0;  // 纯虚函数，让继承类实现，把数据写入音频播放器
	virtual int GetFree() = 0;                                  // 纯虚函数，让继承类实现，是否有足够的空间来进行写
	virtual long long GetNoPlayMs() = 0;                       // 纯虚函数，让继承类实现，有多少pts是没有被播放的  
	virtual void Stop(bool isPause) = 0;

public:
	int sampleRate = 44100;
	int sampleSize = 16;
	int channels = 2;
};


class CXAudioPlay : public XAudioPlay {
public:
	virtual bool Open();
	virtual void Close();
	virtual bool Write(unsigned char* data, int datasize);
	virtual int GetFree();
	virtual long long GetNoPlayMs();
	virtual void Stop(bool isPause);

private:
	std::mutex m_mtx;
	long long pts = 0;

	QAudioOutput *qaudioout = nullptr;
	QIODevice* iodevice = nullptr;
};


#endif 




