#pragma once

#include <GL/glew.h>

#include <QGLWidget>

#include "Renderer.h"

class MyGLWidget : public QGLWidget
{

    Q_OBJECT

private:

	Renderer m_renderer;

public:

	explicit MyGLWidget(QWidget* parent = nullptr);

	MyGLWidget(const MyGLWidget& other) = default;

	MyGLWidget(MyGLWidget&& other) = default;

	MyGLWidget& operator=(const MyGLWidget& other) = default;

	MyGLWidget& operator=(MyGLWidget&& other) = default;

	~MyGLWidget() override = default;

	Renderer& getRenderer();

signals:

	void renderingFinished(float ms);

protected:

    void initializeGL() override;

	void paintGL() override;

	void resizeGL(int width, int heigh) override;

};
