#include "xaudiothread.h"

// std ͷ�ļ� ����
#include <iostream>
using std::cout;
using std::endl;
// ���� ͷ�ļ� ����
#include "xdemux.h"
#include "xdecode.h"
#include "xresample.h"
#include "xaudioplay.h"


XAudioThread::XAudioThread() {
	if (!audioplay) {
		audioplay = XAudioPlay::Get();
	}
	if (!resample) {
		resample = new XResample();
	}
}

XAudioThread::~XAudioThread() {
}

void XAudioThread::Close() {
	XDecodeThread::Close();
	if (resample)
	{
		resample->Close();
		m_a_mtx.lock();
		delete resample;
		resample = nullptr;
		m_a_mtx.unlock();
	}
	if (audioplay)
	{
		audioplay->Close();
		m_a_mtx.lock();
		audioplay = nullptr;
		m_a_mtx.unlock();
	}
}

void XAudioThread::Stop(bool isPause) {
	this->isPause = isPause;
	if (audioplay) {
		audioplay->Stop(isPause);
	}
}

bool XAudioThread::Open(AVCodecParameters* para, int sampleRate, int channels){
	if (!para) return false;
	Clear();

	m_a_mtx.lock();

	bool re = true;
	pts = 0;

	bool ret = resample->Open(para, false);
	if (!ret) {
		cout << "resample open failed! " << endl;
		re = false;
	}
	audioplay->sampleRate = sampleRate;
	audioplay->channels = channels;

	ret = audioplay->Open();
	if (!ret) {
		cout << "audioplay open failed! " << endl;
		re =  false;
	}
                   
	if (!decode->Open(para)) {								// ��һ�� para ���ͷ���
		cout << "audio decode open failed! " << endl;
		re = false;
	}
	m_a_mtx.unlock();
	
	cout << "open audio : " << (re ? " success!" : "failed!") << endl;
	return re;
}

void XAudioThread::run() {
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];
	while (!isExit) {
		m_a_mtx.lock();

		if (isPause) {
			m_a_mtx.unlock();
			msleep(5);
			continue;
		}

		AVPacket* pkt = Pop();
		bool ret = decode->Send(pkt);                                    // ���� pkt �ռ䱻�ͷ�
		if (!ret) {
			m_a_mtx.unlock();
			msleep(1);
			continue;
		}
		while (!isExit) {
			AVFrame* frame = decode->Recv(); 
			if (!frame) break;

			pts = decode->pts - audioplay->GetNoPlayMs();               // decode �� pts (�����) - audioplay��δ���ŵ�pts 
			//cout << "audio pts: " << pts << endl;
			int size = resample->Resample(frame, pcm);                   // ���� frame �ռ䱻�ͷ�
			while (!isExit) {
				if (size <= 0)break;
				if (audioplay->GetFree() < size || isPause) {
					msleep(1);
					continue;
				}
				audioplay->Write(pcm, size);
				break;
			}
		}
		m_a_mtx.unlock();
	}
	delete pcm;
}