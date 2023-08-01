#include "xvideowidget.h"

extern "C"
{

#include <libavutil/frame.h>

}

#define A_VER 3
#define T_VER 4

XVideoWidget::XVideoWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{

}

XVideoWidget::~XVideoWidget()
{
	glDeleteTextures(3, textures);
}

void XVideoWidget::Init(int w, int h)
{
	mux.lock();

	m_width = w;
	m_height = h;

	delete datas[0]; delete datas[1]; delete datas[2];

	datas[0] = new unsigned char[m_width * m_height];//y
	datas[1] = new unsigned char[m_width * m_height / 4];//u
	datas[2] = new unsigned char[m_width * m_height / 4];//v

	if (textures[0])
	{
		glDeleteTextures(3, textures);         // 每次把上次的纹理清理
	}

	// 创建材质
	glGenTextures(3, textures);

	// y
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	// 放大过滤，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 缩小过滤， 线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 创建材质显卡空间
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);


	// uv, 4个y 对应 一组 u,v 根据420 编解码原则来的
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	// 放大过滤，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 缩小过滤， 线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 创建材质显卡空间
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	// 放大过滤，线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// 缩小过滤， 线性插值
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// 创建材质显卡空间
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	mux.unlock();
}

void XVideoWidget::rePaint(AVFrame* frame)
{
	// frame 为空
	if (frame == nullptr) return;

	// 视频帧 尺寸正确
	mux.lock();
	if (!datas[0] || m_width * m_height == 0 || frame->width != m_width || frame->height != m_height)
	{
		av_frame_free(&frame);
		mux.unlock();
		return;
	}
	// 行对齐
	if (m_width == frame->linesize[0]) {
		memcpy(datas[0], frame->data[0], m_width * m_height);
		memcpy(datas[1], frame->data[1], m_width * m_height / 4);
		memcpy(datas[2], frame->data[2], m_width * m_height / 4);
	}
	else {
		for (int i = 0; i < m_height; ++i) {
			memcpy(datas[0] + m_width * i, frame->data[0] + frame->linesize[0] * i, m_width);
		}
		for (int i = 0; i < m_height / 2; ++i) {
			memcpy(datas[0] + m_width * i / 2, frame->data[1] + frame->linesize[1] * i, m_width);
		}
		for (int i = 0; i < m_height / 2; ++i) {
			memcpy(datas[0] + m_width * i / 2, frame->data[2] + frame->linesize[2] * i, m_width);
		}
	}

	av_frame_free(&frame);

	mux.unlock();

	update();
}

void XVideoWidget::initializeGL()
{

	mux.lock();

	// 初始化
	initializeOpenGLFunctions();

	// shader program
	/// 也可以写 define GET_STR(x) #x    这样的宏来做为sourceCode

	bool res = false;

	res = m_program.addShaderFromSourceCode(QOpenGLShader::Vertex, u8R"(
#version 330 core
attribute vec4 vertexIn;
attribute vec2 textureIn;
varying vec2 textureOut;
void main(void)
{
	gl_Position = vertexIn;
	textureOut = textureIn;
}
)");
	res = m_program.addShaderFromSourceCode(QOpenGLShader::Fragment, u8R"(
#version 330 core
varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;

void main(void)
{
	vec3 yuv;
	vec3 rgb;	
	yuv.x = texture2D(tex_y, textureOut).r;
	yuv.y = texture2D(tex_u, textureOut).r - 0.5;
	yuv.z = texture2D(tex_v, textureOut).r - 0.5;

	rgb = mat3(1.0, 1.0, 1.0,
			0.0, -0.39465, 2.03211,
			1.13983, -0.58060, 0.0) * yuv;
	gl_FragColor = vec4(rgb, 1.0);
}
)");

	// 设置顶点坐标变量
	m_program.bindAttributeLocation("vertexIn", A_VER);
	m_program.bindAttributeLocation("textureIn", T_VER);

	res = m_program.link();
	if (!res)
	{
		qDebug() << "program link failed " << endl;
	}
	else
	{
		qDebug() << "program link success " << endl;
	}
	res = m_program.bind();
	if (!res)
	{
		qDebug() << "program bind failed " << endl;
	}
	else
	{
		qDebug() << "program bind success " << endl;
	}

	// 传递顶点和材质坐标, 四个点三维坐标 第三个不写默认为0.0f
	static const GLfloat ver[] = {
		-1.0f,-1.0f,
		1.0f,-1.0f,
		-1.0f, 1.0f,
		1.0f,1.0f
	};

	//材质
	static const GLfloat tex[] =
	{
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	// 顶点坐标
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);// 位置索引，一个顶点大小， 顶点类型， 法线向量， 顶点指针
	glEnableVertexAttribArray(A_VER);
	// 材质
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	glEnableVertexAttribArray(T_VER);

	// 从 shader 中获取材质
	unis[0] = m_program.uniformLocation("tex_y");
	unis[1] = m_program.uniformLocation("tex_u");
	unis[2] = m_program.uniformLocation("tex_v");

	mux.unlock();
}

void XVideoWidget::paintGL()
{
	mux.lock();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);//y 绑定到 GL_TEXTURE0
	// 修改材质内容--复制内存的内容
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
	// 与 shader 的 uniform 关联
	glUniform1i(unis[0], 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);//u 绑定到 GL_TEXTURE1
	// 修改材质内容--复制内存的内容
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
	// 与 shader 的 uniform 关联
	glUniform1i(unis[1], 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);//v 绑定到 GL_TEXTURE2
	// 修改材质内容--复制内存的内容
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
	// 与 shader 的 uniform 关联
	glUniform1i(unis[2], 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	mux.unlock();
}

void XVideoWidget::resizeGL(int w, int h)
{
	mux.lock();
	glViewport(0, 0, w, h);
	mux.unlock();
}