#include "xdecode.h"

// ffmpeg 头文件 引入
extern "C" {

#include <libavcodec/avcodec.h>

}
// std 头文件 引入
#include <iostream>
using std::cout;
using std::endl;

XDecode::XDecode() {
}

XDecode::~XDecode() {
}

void XFreePacket(AVPacket** pkt)
{
	if (pkt || (*pkt))return;
	av_packet_free(pkt);
}

bool XDecode::Open(AVCodecParameters* para) {
	if (!para) return false;                                                   // 没有编码数据
	Close();

	avcodec = avcodec_find_decoder(para->codec_id);              // 选择解码器

	cout << "para->codec_id : " << para->codec_id << ", avcodec : " << avcodec << ", para : " << para << endl;
	if (avcodec == nullptr) {
		avcodec_parameters_free(&para);
		cout << "can not find codec id" << endl;
		return false;
	}
	
	m_mtx.lock();
	codec = avcodec_alloc_context3(avcodec);                                    // 分配 解码器上下文 内存 复制 para 到上下文
	avcodec_parameters_to_context(codec, para);
	codec->thread_count = 8;												   // 配置上下文参数
	
	int ret = avcodec_open2(codec, 0, 0);                                      // 打开上下文
	if (ret != 0) {
		m_mtx.unlock();
		avcodec_free_context(&codec);
		avcodec_parameters_free(&para);
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "avcodec_open2 failed! : " << buf << endl;
		return false;
	}
	avcodec_parameters_free(&para);
	m_mtx.unlock();

	return true;
}

void XDecode::Clear() {
	m_mtx.lock();

	if (codec) {
		avcodec_flush_buffers(codec);
	}
	m_mtx.unlock();
}

void XDecode::Close() {
	m_mtx.lock();

	if (codec) {
		avcodec_close(codec);
		avcodec_free_context(&codec);
	}
	pts = 0;
	m_mtx.unlock();
}

bool XDecode::Send(AVPacket* pkt) {
	if (!pkt || pkt->size <= 0 || !pkt->data) return false;

	m_mtx.lock();

	if (!codec) {
		m_mtx.unlock();
		return false;
	}

	int ret = avcodec_send_packet(codec, pkt);
	m_mtx.unlock();

	av_packet_free(&pkt);
	if (ret != 0)return false;
	return true;
}

AVFrame* XDecode::Recv() {
	m_mtx.lock();

	if (!codec) {
		m_mtx.unlock();
		return nullptr;
	}

	AVFrame* frame = av_frame_alloc();
	int ret = avcodec_receive_frame(codec, frame);
	m_mtx.unlock();
	
	if (ret != 0) {
		av_frame_free(&frame);
		return nullptr;
	}

	pts = frame->pts;

	return frame;
}



