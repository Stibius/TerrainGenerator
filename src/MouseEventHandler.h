#pragma once

#include <QObject>
#include <QPointF>

#include "MyGLWidget.h"

class MouseEventHandler : public QObject
{

	Q_OBJECT

public:

	explicit MouseEventHandler(MyGLWidget* glWidget = nullptr, QObject* parent = nullptr);

	MouseEventHandler(const MouseEventHandler& other) = default;

	MouseEventHandler(MouseEventHandler&& other) = default;

	MouseEventHandler& operator=(const MouseEventHandler& other) = default;

	MouseEventHandler& operator=(MouseEventHandler&& other) = default;

	~MouseEventHandler() override = default;

	void init(MyGLWidget* glWidget, float rotationSpeed = 0.5f, float zoomSpeed = 0.5f);

	void setRotationSpeed(float rotationSpeed);

	void setZoomSpeed(float zoomSpeed);

	bool eventFilter(QObject* obj, QEvent* event) override;

private:

	MyGLWidget* m_glWidget = nullptr;
	bool m_rightMousePressed = false;
	bool m_leftMousePressed = false;
	QPointF m_lastLeftMousePos;
	QPointF m_lastRightMousePos;

	float m_rotationSpeed = 0.5f;
	float m_zoomSpeed = 0.5f;

};
