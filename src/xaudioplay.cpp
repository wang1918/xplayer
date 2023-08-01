#include "xaudioplay.h"

XAudioPlay* XAudioPlay::Get() {
	static CXAudioPlay play;
	return &play;
}

XAudioPlay::XAudioPlay() {

}

XAudioPlay::~XAudioPlay() {

}

void CXAudioPlay::Stop(bool isPause) {
	m_mtx.lock();
	if (!qaudioout) {
		m_mtx.unlock();
		return;
	}
	if (isPause) {
		qaudioout->suspend();
	}
	else {
		qaudioout->resume();
	}
	m_mtx.unlock();
}

bool CXAudioPlay::Open() {
	Close();

	QAudioFormat fmt;
	fmt.setSampleRate(sampleRate);
	fmt.setSampleSize(sampleSize);
	fmt.setChannelCount(channels);
	fmt.setCodec("audio/pcm");
	fmt.setByteOrder(QAudioFormat::LittleEndian);
	fmt.setSampleType(QAudioFormat::UnSignedInt);

	m_mtx.lock();

	qaudioout = new QAudioOutput(fmt);
	iodevice = qaudioout->start();
	m_mtx.unlock();
	if (iodevice) return true;
	return false;

}

void CXAudioPlay::Close() {
	m_mtx.lock();

	if (iodevice) {
		iodevice->close();
		iodevice = nullptr;
	}

	if (qaudioout) {
		qaudioout->stop();
		delete qaudioout;
		qaudioout = nullptr;
	}

	m_mtx.unlock();
}

bool CXAudioPlay::Write(unsigned char* data, int datasize) {
	if (!data || datasize <= 0) return false;
	m_mtx.lock();
	if (!qaudioout || !iodevice) {
		m_mtx.unlock();
		return false;
	}
	int size = iodevice->write((char*)data, datasize);
	m_mtx.unlock();
	if (datasize != size) return false;
	return true;
}

int CXAudioPlay::GetFree() {
	m_mtx.lock();

	if (!qaudioout) {
		m_mtx.unlock();
		return 0;
	}
	int free = qaudioout->bytesFree();
	m_mtx.unlock();

	return free;
}

long long CXAudioPlay::GetNoPlayMs() {
	m_mtx.lock();
	if (!qaudioout) {
		m_mtx.unlock();
		return 0;
	}

	int size = qaudioout->bufferSize() - qaudioout->bytesFree();      // 还没有被播放的字节数大小
	double secSize = sampleRate * (sampleSize / 8.0) * channels;      // 一秒钟的字节数大小
	if (secSize <= 0) {
		pts = 0;
	}
	else {
		pts = (size / secSize) * 1000;
	}
	m_mtx.unlock();

	return pts;
}


