#include "xresample.h"
// ffmpeg 头文件 引入
extern "C" {

#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>

}
// std 头文件 引入
#include <iostream>
using std::cout;
using std::endl;


XResample::XResample(){

}

XResample::~XResample(){

}

bool XResample::Open(AVCodecParameters* para, bool ClearPara) {
	if (!para) {
		return false;
	}
	m_mtx.lock();

	swrc = swr_alloc();
	swrc = swr_alloc_set_opts(swrc,
		av_get_default_channel_layout(2),								// 输出格式
		(AVSampleFormat)output_sampleformat,                            // 输出样本格式
		para->sample_rate,                                              // 输出采样率
		av_get_default_channel_layout(para->channels),                  // 输入格式
		(AVSampleFormat)para->format,                                   // 输入样本格式                               
		para->sample_rate,                                              // 输入采用率
		0, 0
		);
	sampleRate = para->sample_rate;
	channels = para->channels;
	if (ClearPara) {
		avcodec_parameters_free(&para);
	}
	int ret = swr_init(swrc);
	m_mtx.unlock();

	if (ret != 0) {
		
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << "swr_init failed! : " << buf << endl;
		return false;
	}
	return true;
}

void XResample::Close() {
	m_mtx.lock();

	m_mtx.unlock();
}

int XResample::Resample(AVFrame* frame, unsigned char* data) {
	if (!frame) return 0;
	if (!data) {
		av_frame_free(&frame);
		return 0;
	}

	uint8_t* datas[2] = { 0 };
	datas[0] = data;
	int ret = swr_convert(swrc,
						  datas,                                        // 输出数据
						  frame->nb_samples,                            // 输出采样数
						  (const uint8_t**)frame->data,                 // 输入数据
						  frame->nb_samples);                           // 输入采样数
	
	if (ret <= 0) {
		av_frame_free(&frame);
		return ret;
	}
	int outsize =  ret * frame->channels * av_get_bytes_per_sample((AVSampleFormat)output_sampleformat);
	return outsize;
}