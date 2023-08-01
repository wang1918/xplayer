#ifndef XDECODE_H
#define XDECODE_H

// std ͷ�ļ� ����
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct AVCodec;

extern void XFreePacket(AVPacket** pkt);

// �������ݰ�
class XDecode
{
public:
	XDecode();
	virtual ~XDecode();

	virtual bool Open(AVCodecParameters* para);                      // �򿪽�����
	virtual void Clear();                                            // ������뻺��
	virtual void Close();                                            // �رս�����
	virtual bool Send(AVPacket* pkt);                                // ���� AVPacket ���߳� �ͷ�pkt
	virtual AVFrame* Recv();                                         // ��ȡ�������ݣ�һ��Send ���Recv

	long long pts = 0;                                               // ������Ƶͬ��

protected:
	std::mutex m_mtx;

	const AVCodec* avcodec = nullptr;
	AVCodecContext* codec = nullptr;
};


#endif 



