
#include <QEvent>
#include <QMouseEvent>

#include "MouseEventHandler.h"

MouseEventHandler::MouseEventHandler(MyGLWidget* glWidget, QObject* parent)
	: QObject(parent)
{
	init(glWidget);
}

void MouseEventHandler::init(MyGLWidget* glWidget, float rotationSpeed, float zoomSpeed)
{
	m_glWidget = glWidget;
	m_rotationSpeed = rotationSpeed;
	m_zoomSpeed = zoomSpeed;
}

void MouseEventHandler::setRotationSpeed(float rotationSpeed)
{
	m_rotationSpeed = rotationSpeed;
}

void MouseEventHandler::setZoomSpeed(float zoomSpeed)
{
	m_zoomSpeed = zoomSpeed;
}

bool MouseEventHandler::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == Qt::LeftButton)
		{
			m_leftMousePressed = true;
			m_lastLeftMousePos = mouseEvent->pos();

			return true;
		}
		else if (mouseEvent->button() == Qt::RightButton)
		{
			m_rightMousePressed = true;
			m_lastRightMousePos = mouseEvent->pos();

			return true;
		}
	}
	else if (event->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		if (mouseEvent->button() == Qt::LeftButton)
		{
			m_leftMousePressed = false;

			return true;
		}
		else if (mouseEvent->button() == Qt::RightButton)
		{
			m_rightMousePressed = false;

			return true;
		}
	}
	else if (event->type() == QEvent::MouseMove)
	{
		QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

		bool change = false;

		OrbitPerspectiveCamera camera = m_glWidget->getRenderer().getCamera();

		if (m_leftMousePressed == true)
		{
			QPointF diff = mouseEvent->pos() - m_lastLeftMousePos;
			m_lastLeftMousePos = mouseEvent->pos();

			camera.m_orbitCamera.addRotation(glm::vec2(glm::radians(diff.y()) * m_rotationSpeed, glm::radians(diff.x()) * m_rotationSpeed));
			change = true;
		}

		if (m_rightMousePressed == true)
		{
			float diff = mouseEvent->pos().y() - m_lastRightMousePos.y();
			m_lastRightMousePos = mouseEvent->pos();

			camera.m_orbitCamera.addDistance(diff * m_zoomSpeed);
			change = true;
		}

		if (change == true)
		{
			m_glWidget->getRenderer().setCamera(camera);
			m_glWidget->updateGL();
		}

		return true;

	}

	return QObject::eventFilter(obj, event);
}
