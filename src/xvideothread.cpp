#include "xvideothread.h"

// std ͷ�ļ� ����
#include <iostream>
using std::cout;
using std::endl;
// ���� ͷ�ļ� ����
#include "xdemux.h"
#include "xdecode.h"
#include "xvideowidget.h"

XVideoThread::XVideoThread() {

}

XVideoThread::~XVideoThread() {

}


bool XVideoThread::Open(AVCodecParameters* para, IVideoCall* videocall, int width, int height) {
	if (!para)return false;
	Clear();
	
	m_v_mtx.lock();

	bool re = true;
	synpts = 0;
	call = videocall;
	if (call) {
		call->Init(width, height);  // Ϊ�˲�����ffmpegͷ�ļ�
	}
	m_v_mtx.unlock();

	if (!decode->Open(para)) {
		m_v_mtx.unlock();
		cout << "video decode open failed! " << endl;
		re = false;
	}

	cout << "open video : " << (re ? " success!" : "failed!") << endl;
	return re;
}

void XVideoThread::Stop(bool isPause) {
	this->isPause = isPause;
}

void XVideoThread::run() {
	while (!isExit) {
		m_v_mtx.lock();
		if (isPause) {
			m_v_mtx.unlock();
			msleep(5);
			continue;
		}

		if (synpts > 0 && synpts < decode->pts) {// ����Ƶͬ��
			m_v_mtx.unlock();
			msleep(1);
			continue;
		}
		AVPacket* pkt = Pop();

		bool ret = decode->Send(pkt);                                    // ���� pkt �ռ䱻�ͷ�
		if (!ret) {
			m_v_mtx.unlock();
			msleep(1);
			continue;
		}
		while (!isExit) {
			AVFrame* frame = decode->Recv();
			if (frame == nullptr) break;
			if(call) 
				call->rePaint(frame);   
		}
		m_v_mtx.unlock();
	}
}

