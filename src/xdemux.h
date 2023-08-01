#ifndef XDEMUX_H
#define XDEMUX_H

// std 头文件 引入
#include <mutex>
struct AVFormatContext;
struct AVPacket;
struct AVCodecParameters;

// 虚基类，用于打开媒体文件 将文件编码为编码数据包
class XDemux
{
public:
	XDemux();
	virtual ~XDemux();

	virtual bool Open(const char* url);			// 用于打开本地文件、 流媒体，rtmp、http、rstp
	virtual AVPacket* Read();                   // 在 Read 中开辟空间，由调用者释放 av_packet_free
	AVCodecParameters* CpVPara();               // 赋值视频参数，调用者要释放视频参数内存 avcodec_parameters_free
	AVCodecParameters* CpAPara();               // 赋值音频参数
	virtual bool Seek(double pos);              // pos [0.0, 1.0] 视频跳转
	virtual void Clear();                       // 清空缓存
	virtual void Close();                       // 关闭
	virtual bool IsAudio(AVPacket* pkt);        // 判断是音频还是视频

	int video_width = 0;                        // 视频宽高
	int video_height = 0;
	int totalMs = 0;							// 视频总时长
	double video_fps = 0;                       // 视频帧率

	int sampleRate = 0;                         // 音频信息
	int channels = 0;

protected:
	std::mutex m_mtx;
	AVFormatContext* ic = nullptr;				// 解封装上下文
	int videoStream = 0;					    // 音视频索引
	int audioStream = 1;
};

#endif