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
		glDeleteTextures(3, textures);         // ÿ�ΰ��ϴε���������
	}

	// ��������
	glGenTextures(3, textures);

	// y
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	// �Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// ��С���ˣ� ���Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);


	// uv, 4��y ��Ӧ һ�� u,v ����420 �����ԭ������
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	// �Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// ��С���ˣ� ���Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	glBindTexture(GL_TEXTURE_2D, textures[2]);
	// �Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// ��С���ˣ� ���Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width / 2, m_height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	mux.unlock();
}

void XVideoWidget::rePaint(AVFrame* frame)
{
	// frame Ϊ��
	if (frame == nullptr) return;

	// ��Ƶ֡ �ߴ���ȷ
	mux.lock();
	if (!datas[0] || m_width * m_height == 0 || frame->width != m_width || frame->height != m_height)
	{
		av_frame_free(&frame);
		mux.unlock();
		return;
	}
	// �ж���
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

	// ��ʼ��
	initializeOpenGLFunctions();

	// shader program
	/// Ҳ����д define GET_STR(x) #x    �����ĺ�����ΪsourceCode

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

	// ���ö����������
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

	// ���ݶ���Ͳ�������, �ĸ�����ά���� ��������дĬ��Ϊ0.0f
	static const GLfloat ver[] = {
		-1.0f,-1.0f,
		1.0f,-1.0f,
		-1.0f, 1.0f,
		1.0f,1.0f
	};

	//����
	static const GLfloat tex[] =
	{
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	// ��������
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);// λ��������һ�������С�� �������ͣ� ���������� ����ָ��
	glEnableVertexAttribArray(A_VER);
	// ����
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	glEnableVertexAttribArray(T_VER);

	// �� shader �л�ȡ����
	unis[0] = m_program.uniformLocation("tex_y");
	unis[1] = m_program.uniformLocation("tex_u");
	unis[2] = m_program.uniformLocation("tex_v");

	mux.unlock();
}

void XVideoWidget::paintGL()
{
	mux.lock();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);//y �󶨵� GL_TEXTURE0
	// �޸Ĳ�������--�����ڴ������
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
	// �� shader �� uniform ����
	glUniform1i(unis[0], 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);//u �󶨵� GL_TEXTURE1
	// �޸Ĳ�������--�����ڴ������
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
	// �� shader �� uniform ����
	glUniform1i(unis[1], 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);//v �󶨵� GL_TEXTURE2
	// �޸Ĳ�������--�����ڴ������
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width / 2, m_height / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
	// �� shader �� uniform ����
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