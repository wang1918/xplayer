#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

// QT 头文件引入
#include <qtimer.h>
#include <qdebug.h>
#include <qopenglwidget.h>
#include <qopenglfunctions.h>
#include <qopenglshaderprogram.h>
// std 头文件引入
#include <mutex>
// 本地文件引入
#include "ivideocall.h"   // 这里只声明 class IVideoCall 不行

struct AVFrame;

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
	Q_OBJECT
public:
	explicit XVideoWidget(QWidget* parent);
	~XVideoWidget();

	virtual void Init(int w, int h);                            // 初始化
	virtual void rePaint(AVFrame* frame);               // 不管成功与否，都释放 frame 空间

protected:
	void paintGL() override;                            // 刷新显示
	void initializeGL() override;                       // 初始化
	void resizeGL(int w, int h) override;               // 窗口缩放

private:
	QOpenGLShaderProgram m_program;
	GLuint unis[3] = { 0 };
	GLuint textures[3] = { 0 };

	std::mutex mux;

	// 材质的内存空间 
	unsigned char* datas[3] = { 0 };

	int m_width;
	int m_height;
};


#endif 



