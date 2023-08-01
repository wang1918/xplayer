#ifndef IVIDEOCALL_H
#define IVIDEOCALL_H

struct AVFrame;

class IVideoCall {
public:
	virtual void Init(int w, int h) = 0;
	virtual void rePaint(AVFrame* frame) = 0;
};


#endif // !IVIDEOCALL_H
