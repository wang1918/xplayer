#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

// QT ͷ�ļ�����
#include <qtimer.h>
#include <qdebug.h>
#include <qopenglwidget.h>
#include <qopenglfunctions.h>
#include <qopenglshaderprogram.h>
// std ͷ�ļ�����
#include <mutex>
// �����ļ�����
#include "ivideocall.h"   // ����ֻ���� class IVideoCall ����

struct AVFrame;

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
	Q_OBJECT
public:
	explicit XVideoWidget(QWidget* parent);
	~XVideoWidget();

	virtual void Init(int w, int h);                            // ��ʼ��
	virtual void rePaint(AVFrame* frame);               // ���ܳɹ���񣬶��ͷ� frame �ռ�

protected:
	void paintGL() override;                            // ˢ����ʾ
	void initializeGL() override;                       // ��ʼ��
	void resizeGL(int w, int h) override;               // ��������

private:
	QOpenGLShaderProgram m_program;
	GLuint unis[3] = { 0 };
	GLuint textures[3] = { 0 };

	std::mutex mux;

	// ���ʵ��ڴ�ռ� 
	unsigned char* datas[3] = { 0 };

	int m_width;
	int m_height;
};


#endif 



