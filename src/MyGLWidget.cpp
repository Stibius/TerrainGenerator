
#include "MyGLWidget.h"

MyGLWidget::MyGLWidget(QWidget* parent)
	: QGLWidget(parent)
{

}

Renderer& MyGLWidget::getRenderer() 
{
	return m_renderer;
}

void MyGLWidget::initializeGL()
{
	m_renderer.init();
}

void MyGLWidget::paintGL()
{
	int micro = m_renderer.render();

	emit renderingFinished(micro / 1000.0f);
}

void MyGLWidget::resizeGL(int width, int height)
{
	m_renderer.setViewportSize(width, height);
}

