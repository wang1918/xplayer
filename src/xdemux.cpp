#include "xdemux.h"

// ffmpeg 头文件 引入
extern "C" {

#include <libavformat/avformat.h>

}

// std 头文件 引入
#include <iostream>
using std::cout;
using std::endl;


// ffmpeg 库文件 引入
//#pragma comment(lib, "avformat.lib")                     // 在环境中配置了

// 静态函数，pts 转 ms
static double r2d(AVRational r);
// 静态函数，打印视频流信息
static void prtAinfo(AVStream* as);
static void prtVinfo(AVStream* vs);


XDemux::XDemux() {
	static bool isFrist = true;                            // 初始化 封装库、网络库，用锁防止多线程多次初始化
	static std::mutex mtx;
	mtx.lock();
	if (isFrist) { 
		avformat_network_init();
		isFrist = false;
	}
	mtx.unlock();
}
XDemux::~XDemux() {

}

bool XDemux::Open(const char* url) {
	Close();
	AVDictionary* opts = nullptr;                          // 参数设置
	av_dict_set(&opts, "rtsp_transport", "tcp", 0);        // 设置rtsp流 tcp协议已打开
	av_dict_set(&opts, "max_delay", "500", 0);             // 设置网络延时时间

	m_mtx.lock();

	// 解封装上下文
	int ret = avformat_open_input(&ic, url, 0, &opts);     // 0 表示自动选择解封器
	if (ret != 0) {
		m_mtx.unlock();

		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "open " << url << " failed! : " << buf << endl;
		return false;
	}
	
	ret = avformat_find_stream_info(ic, 0);                // 获取流信息
	totalMs = ic->duration / (AV_TIME_BASE / 1000);        // 视频总时长 ms
	cout << "open success, video total time = " << totalMs << " ms." << endl;
	//av_dump_format(ic, 0, url, 0);                         // 打印视频流详细信息
	// 获取视频流
	videoStream = av_find_best_stream(ic, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0); 
	AVStream* vs = ic->streams[videoStream];
	video_width = vs->codecpar->width;
	video_height = vs->codecpar->height;
	video_fps = r2d(vs->avg_frame_rate);
	// 获取音频流
	audioStream = av_find_best_stream(ic, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
	AVStream* as = ic->streams[audioStream];
	sampleRate = as->codecpar->sample_rate;
	channels = as->codecpar->channels;

	//prtAinfo(as);                                          // 打印信息
	//prtVinfo(vs);
	m_mtx.unlock();

	return true;
}

AVPacket* XDemux::Read() {
	m_mtx.lock();

	if (!ic) {
		m_mtx.unlock();
		return nullptr;
	}

	AVPacket* pkt = av_packet_alloc();
	int ret = av_read_frame(ic, pkt);                     // 读取一帧，并分配一帧的空间
	if (ret != 0) {                                       // 读取失败，释放空间
		m_mtx.unlock();

		av_packet_free(&pkt);
		return nullptr;
	}
	// 把 pts dts 转化为 ms 为单位的值
	pkt->pts = pkt->pts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	pkt->dts = pkt->dts * (1000 * (r2d(ic->streams[pkt->stream_index]->time_base)));
	//cout << "pkt->pts : " << pkt->pts << endl;
	//cout << "pkt->dts : " << pkt->dts << endl;

	m_mtx.unlock();
	return pkt;
}

AVCodecParameters* XDemux::CpVPara() {
	m_mtx.lock();
	
	if (!ic) {
		m_mtx.unlock();
		return nullptr;
	}
	AVCodecParameters* vpa = avcodec_parameters_alloc();
	avcodec_parameters_copy(vpa, ic->streams[videoStream]->codecpar);
	m_mtx.unlock();

	return vpa;
}

AVCodecParameters* XDemux::CpAPara() {
	m_mtx.lock();

	if (!ic) {
		m_mtx.unlock();
		return nullptr;
	}
	AVCodecParameters* apa = avcodec_parameters_alloc();
	avcodec_parameters_copy(apa, ic->streams[audioStream]->codecpar);
	m_mtx.unlock();

	return apa;
}


bool XDemux::Seek(double pos) {
	m_mtx.lock();
	
	if (!ic) {
		m_mtx.unlock();
		return false;
	}

	avformat_flush(ic);                                   // 清理读取缓冲

	long long seekpos = 0;
	int duration = ic->streams[videoStream]->duration;
	if (duration > 0) {
		seekpos =  duration * pos;
	}
	else {
		seekpos = totalMs * pos / (double)1000 * r2d(ic->streams[videoStream]->time_base);
	}
	// 只是跳到关键帧，没有跳到实际帧
	int ret = av_seek_frame(ic, videoStream, seekpos, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	m_mtx.unlock();
	
	if (ret < 0) return false;
	return true;
}

void XDemux::Clear() {
	m_mtx.lock();

	if (!ic) {
		m_mtx.unlock();
		return;
	}
	avformat_flush(ic);
	m_mtx.unlock();
}

void XDemux::Close() {
	m_mtx.lock();

	if (!ic) {
		m_mtx.unlock();
		return;
	}
	avformat_close_input(&ic);
	totalMs = 0;
	m_mtx.unlock();
}

bool XDemux::IsAudio(AVPacket* pkt) {
	if (!pkt) {
		return false;
	}
	if (pkt->stream_index == videoStream) {
		return false;
	}
	return true;
}

// 静态函数，pts 转 ms
static double r2d(AVRational r) {
	return r.den == 0 ? 0 : (double)r.num / (double)r.den;
}

// 静态函数，打印视频流信息
static void prtAinfo(AVStream* as) {
	cout << "============  音频信息  ============" << endl;
	cout << "codec_id = " << as->codecpar->codec_id << endl;
	cout << "format = " << as->codecpar->format << endl;
	cout << "sample rate = " << as->codecpar->sample_rate << endl;
	cout << "channels = " << as->codecpar->channels << endl;
	cout << "frame size = " << as->codecpar->frame_size << endl;
}
static void prtVinfo(AVStream* vs) {
	cout << "============  视频信息  ============" << endl;
	cout << "codec_id = " << vs->codecpar->codec_id << endl;
	cout << "format = " << vs->codecpar->format << endl;
	cout << "width = " << vs->codecpar->width << ", height = " << vs->codecpar->height << endl;
	cout << "video fps = " << r2d(vs->avg_frame_rate) << endl;
}


