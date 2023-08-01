#ifndef XDEMUX_H
#define XDEMUX_H

// std ͷ�ļ� ����
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

// ����࣬���ڴ�ý���ļ� ���ļ�����Ϊ�������ݰ�
class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	virtual bool Open(const char* url);			// ���ڴ򿪱����ļ��� ��ý�壬rtmp��http��rstp
	virtual AVPacket* Read();                   // �� Read �п��ٿռ䣬�ɵ������ͷ� av_packet_free
	AVCodecParameters* CpVPara();               // ��ֵ��Ƶ������������Ҫ�ͷ���Ƶ�����ڴ� avcodec_parameters_free
	AVCodecParameters* CpAPara();               // ��ֵ��Ƶ����
	virtual bool Seek(double pos);              // pos [0.0, 1.0] ��Ƶ��ת
	virtual void Clear();                       // ��ջ���
	virtual void Close();                       // �ر�
	virtual bool IsAudio(AVPacket* pkt);        // �ж�����Ƶ������Ƶ

	int video_width = 0;                        // ��Ƶ���
	int video_height = 0;
	int totalMs = 0;							// ��Ƶ��ʱ��
	double video_fps = 0;                       // ��Ƶ֡��

	int sampleRate = 0;                         // ��Ƶ��Ϣ
	int channels = 0;

protected:
	std::mutex m_mtx;
	AVFormatContext* ic = nullptr;				// ���װ������
	int videoStream = 0;					    // ����Ƶ����
	int audioStream = 1;
};

#endif