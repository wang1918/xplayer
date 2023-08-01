#ifndef XDECODE_H
#define XDECODE_H

// std 头文件 引入
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct AVCodec;

extern void XFreePacket(AVPacket** pkt);

// 解码数据包
class XDecode
{
public:
	XDecode();
	virtual ~XDecode();

	virtual bool Open(AVCodecParameters* para);                      // 打开解码器
	virtual void Clear();                                            // 清理解码缓冲
	virtual void Close();                                            // 关闭解码器
	virtual bool Send(AVPacket* pkt);                                // 发送 AVPacket 到线程 释放pkt
	virtual AVFrame* Recv();                                         // 获取解码数据，一次Send 多次Recv

	long long pts = 0;                                               // 做音视频同步

protected:
	std::mutex m_mtx;

	const AVCodec* avcodec = nullptr;
	AVCodecContext* codec = nullptr;
};


#endif 



