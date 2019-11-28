#pragma once

#include <QApplication>
#include <QColor>
#include <QImage>
#include <QPixmap>
#include <QString>
#include <QVector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ConcurrencyHandler.h"
#include "Heightmap.h"
#include "MainWindow.h"
#include "MouseEventHandler.h"

class Application
{

public:

	struct MaterialTexture
	{
		QImage m_image;
		QString m_filename;
	};

	static bool m_initialized;
	static std::unique_ptr<QApplication> m_qApplication;
	static std::unique_ptr<MainWindow> m_mainWindow;
	static MouseEventHandler m_mouseEventHandler;
	static ConcurrencyHandler m_concurrencyHandler;
	static Heightmap m_heightmap;
	static Heightmap m_heightmapOrig;
	static QVector<MaterialTexture> m_materialTextures;

	static void init(int& argc, char* argv[]);

	static int run();

	static void cleanUp();

};
