#include "xdecodethread.h"

// std ͷ�ļ� ����
#include <iostream>
using std::cout;
using std::endl;
// ���� ͷ�ļ� ����
#include "xdemux.h"
#include "xdecode.h"
#include "xvideowidget.h"

XDecodeThread::XDecodeThread() {
	//�򿪽�����
	if (!decode) decode = new XDecode();
}

XDecodeThread::~XDecodeThread() {
	//�ȴ��߳��˳�
	isExit = true;
	wait();
}

void XDecodeThread::Push(AVPacket* pkt) {
	if (!pkt) return;

	while (!isExit) {
		m_mtx.lock();
		if (m_pkt_que.size() < maxSize) {
			m_pkt_que.push(pkt);
			m_mtx.unlock();
			break;
		}
		m_mtx.unlock();
		msleep(1);
	}
}

AVPacket* XDecodeThread::Pop() {
	m_mtx.lock();
	if (m_pkt_que.empty()) {
		m_mtx.unlock();
		return nullptr;
	}

	AVPacket* pkt = m_pkt_que.front();
	m_pkt_que.pop();
	m_mtx.unlock();
	return pkt;
}

void XDecodeThread::Clear() {
	m_mtx.lock();
	decode->Clear();
	while (!m_pkt_que.empty()) {
		AVPacket* pkt = m_pkt_que.front();
		XFreePacket(&pkt);
		m_pkt_que.pop();
	}
	m_mtx.unlock();
}

void XDecodeThread::Close() {
	Clear();
	// �ȴ��߳��˳�
	isExit = true;
	wait();
	decode->Close();

	m_mtx.lock();
	delete decode;
	decode = nullptr;
	m_mtx.unlock();
}
