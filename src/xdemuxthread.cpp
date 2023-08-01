#include "xdemuxthread.h"

// std 头文件 引入
#include <iostream>
using std::cout;
using std::endl;
// 本地 头文件 引入
#include "xdemux.h"
#include "xaudiothread.h"
#include "xvideothread.h"

XDemuxThread::XDemuxThread() {
	if (!demux) {
		demux = new XDemux();
	}
	if (!audiothread) {
		audiothread = new XAudioThread();
	}
	if (!videothread) {
		videothread = new XVideoThread();
	}
}

XDemuxThread::~XDemuxThread() {
	isExit = true;
	wait();
}

bool XDemuxThread::Open(const char* url, IVideoCall* call) {
	if (url == 0 || url[0] == '\0') return false;

	m_mtx.lock();

	bool re = true;
	re = demux->Open(url);
	if (!re) {
		m_mtx.unlock();
		cout << "demux open " << url << " failed!" << endl;
		return false;
	}

	re = videothread->Open(demux->CpVPara(), call, demux->video_width, demux->video_height);
	totalMs = demux->totalMs;
	if (!re) {
		m_mtx.unlock();
		cout << "videothread open failed! " << endl;
		re = false;
	}

	re = audiothread->Open(demux->CpAPara(), demux->sampleRate, demux->channels);
	if (!re) {
		m_mtx.unlock();
		cout << "audiothread open failed! " << endl;
		re = false;
	}
	m_mtx.unlock();

	cout << "xdemux open : " << (re ? " success!" : "failed!") << endl;
	return re;
}

void XDemuxThread::Start() {
	m_mtx.lock();

	this->start();
	if (audiothread != nullptr) {
		audiothread->start();
	}
	if (videothread != nullptr) {
		videothread->start();
	}

	m_mtx.unlock();

}

void XDemuxThread::Stop(bool isPause) {
	m_mtx.lock();
	this->isPause = isPause;
	if (audiothread) audiothread->Stop(isPause);
	if (videothread) videothread->Stop(isPause);
	m_mtx.unlock();
}

void XDemuxThread::Close() {
	isExit = true;
	wait();
	if (audiothread) {
		audiothread->Close();
	}
	if (videothread) {
		videothread->Close();
	}
	m_mtx.lock();
	delete audiothread; audiothread = nullptr;
	delete videothread; videothread = nullptr;
	m_mtx.unlock();
}

void XDemuxThread::run() {
	while (!isExit) {
		
		m_mtx.lock();
		if (isPause) {
			m_mtx.unlock();
			msleep(5);
			continue;
		}

		
		if (demux == nullptr) {
			m_mtx.unlock();
			msleep(5);
			continue;
		}
		if (audiothread != nullptr && videothread != nullptr) {  // 音视频同步
			pts = audiothread->pts;
			videothread->synpts = audiothread->pts;
		}

		AVPacket* pkt = demux->Read();
		if (pkt == nullptr) {
			m_mtx.unlock();
			msleep(5);
			continue;
		}

		if (demux->IsAudio(pkt)) {
			if (audiothread) {
				audiothread->Push(pkt);
			}
		}
		else {
			if (videothread) {
				videothread->Push(pkt);
			}
		}
		m_mtx.unlock();
	}
}
