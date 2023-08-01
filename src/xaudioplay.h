#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H
// QT ͷ�ļ�����
#include <qaudiooutput.h>
#include <qaudiodeviceinfo.h>
// STD ͷ�ļ�����
#include <mutex>

class XAudioPlay
{
public:
	static XAudioPlay* Get();                                   // ����ģʽ�����������Ĳ��ŷ�ʽ���Է���Get����������ģʽ
	XAudioPlay();
	virtual ~XAudioPlay();

	virtual bool Open() = 0;                                    // ���麯�����ü̳���ʵ�֣���QT����Ƶ����
	virtual void Close() = 0;                                   // ���麯�����ü̳���ʵ�֣��ر���Ƶ���ţ�Ϊ���δ�
	virtual bool Write(unsigned char* data, int datasize) = 0;  // ���麯�����ü̳���ʵ�֣�������д����Ƶ������
	virtual int GetFree() = 0;                                  // ���麯�����ü̳���ʵ�֣��Ƿ����㹻�Ŀռ�������д
	virtual long long GetNoPlayMs() = 0;                       // ���麯�����ü̳���ʵ�֣��ж���pts��û�б����ŵ�  
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




