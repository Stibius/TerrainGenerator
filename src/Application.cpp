
#include "Application.h"
#include "FileLoader.h"

bool Application::m_initialized = false;
std::unique_ptr<QApplication> Application::m_qApplication;
std::unique_ptr<MainWindow> Application::m_mainWindow;
MouseEventHandler Application::m_mouseEventHandler;
ConcurrencyHandler Application::m_concurrencyHandler;
Heightmap Application::m_heightmap;
Heightmap Application::m_heightmapOrig;
QVector<Application::MaterialTexture> Application::m_materialTextures;

void Application::init(int& argc, char* argv[])
{
	if (m_initialized == true)
	{
		return;
	}
	m_initialized = true;

	m_qApplication = std::make_unique<QApplication>(argc, argv);
	m_mainWindow = std::make_unique<MainWindow>();

	MyGLWidget& glWidget = m_mainWindow->getGLWidget();
	Renderer& renderer = glWidget.getRenderer();

	m_mouseEventHandler.init(&glWidget);
	glWidget.installEventFilter(&m_mouseEventHandler);

	QObject::connect(m_mainWindow.get(), &MainWindow::generateHeightmapCircles, &m_concurrencyHandler, &ConcurrencyHandler::onGenerateHeightmapCircles);
	QObject::connect(m_mainWindow.get(), &MainWindow::generateHeightmapPerlin, &m_concurrencyHandler, &ConcurrencyHandler::onGenerateHeightmapPerlin);
	QObject::connect(m_mainWindow.get(), &MainWindow::generateHeightmapDiamond, &m_concurrencyHandler, &ConcurrencyHandler::onGenerateHeightmapDiamond);
	QObject::connect(m_mainWindow.get(), &MainWindow::generateHeightmapFault, &m_concurrencyHandler, &ConcurrencyHandler::onGenerateHeightmapFault);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::heightmapGenerated, m_mainWindow.get(), &MainWindow::onHeightmapGenerated);
	QObject::connect(m_mainWindow.get(), &MainWindow::createMesh, &m_concurrencyHandler, &ConcurrencyHandler::onCreateMesh);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::meshCreated, m_mainWindow.get(), &MainWindow::onMeshCreated);
	QObject::connect(m_mainWindow.get(), &MainWindow::loadMesh, &m_concurrencyHandler, &ConcurrencyHandler::onLoadMesh);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::meshLoaded, m_mainWindow.get(), &MainWindow::onMeshLoaded);
	QObject::connect(m_mainWindow.get(), &MainWindow::saveMesh, &m_concurrencyHandler, &ConcurrencyHandler::onSaveMesh);
	QObject::connect(&m_concurrencyHandler, &ConcurrencyHandler::meshSaved, m_mainWindow.get(), &MainWindow::onMeshSaved);

	std::string vs1 = FileLoader::loadFile(":/shaders/terrain.vert");
	std::string fs1 = FileLoader::loadFile(":/shaders/terrain.frag");
	std::string vs2 = FileLoader::loadFile(":/shaders/depthMap.vert");
	std::string fs2 = FileLoader::loadFile(":/shaders/depthMap.frag");
	renderer.setShaders(vs1, fs1, vs2, fs2);
}

int Application::run()
{
	if (m_initialized == false)
	{
		return -1;
	}

	m_mainWindow->showMaximized();

	return m_qApplication->exec();
}

void Application::cleanUp()
{
	if (m_initialized == false)
	{
		return;
	}
	m_initialized = false;

	m_qApplication = nullptr;
	m_mainWindow = nullptr;
}
