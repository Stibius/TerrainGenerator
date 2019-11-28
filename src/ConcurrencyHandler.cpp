
#include "ConcurrencyHandler.h"

ConcurrencyHandler::ConcurrencyHandler(QObject* parent)
	: QObject(parent)
{

}

void ConcurrencyHandler::onGenerateHeightmapDiamond(int iterations, int startAmplitude, float amplitudeModifier)
{
	if (m_connected == false)
	{
		connect();
	}

	m_generateHeightmapFuture = QtConcurrent::run([iterations, startAmplitude, amplitudeModifier]()
	{
		return HeightmapGenerator::generateDiamond(iterations, startAmplitude, amplitudeModifier);
	});

	m_generateHeightmapFutureWatcher.setFuture(m_generateHeightmapFuture);
}

bool ConcurrencyHandler::isGeneratingHeightmap() const
{
	return m_generateHeightmapFuture.isRunning();
}

bool ConcurrencyHandler::isCreatingMesh() const
{
	return m_createMeshFuture.isRunning();
}

bool ConcurrencyHandler::isLoadingMesh() const
{
	return m_loadMeshFuture.isRunning();
}

bool ConcurrencyHandler::isSavingMesh() const
{
	return m_saveMeshFuture.isRunning();
}

void ConcurrencyHandler::onGenerateHeightmapCircles(int width, int height, int minRadius, int maxRadius, int minAmplitude, int maxAmplitude, int iterations)
{
	if (m_connected == false)
	{
		connect();
	}

	m_generateHeightmapFuture = QtConcurrent::run([width, height, minRadius, maxRadius, minAmplitude, maxAmplitude, iterations]()
	{
		return HeightmapGenerator::generateCircles(
			width,
			height,
			minRadius,
			maxRadius,
			minAmplitude,
			maxAmplitude,
			iterations);
	});

	m_generateHeightmapFutureWatcher.setFuture(m_generateHeightmapFuture);
}

void ConcurrencyHandler::onGenerateHeightmapPerlin(int size, int octaves, float amplitudeModifier, float frequencyModifier)
{
	if (m_connected == false)
	{
		connect();
	}

	m_generateHeightmapFuture = QtConcurrent::run([size, octaves, amplitudeModifier, frequencyModifier]()
	{
		return HeightmapGenerator::generatePerlin(
			size,
			octaves,
			amplitudeModifier,
			frequencyModifier);
	});

	m_generateHeightmapFutureWatcher.setFuture(m_generateHeightmapFuture);
}

void ConcurrencyHandler::onGenerateHeightmapFault(int width, int height, int iterations, int startAmplitude, int endAmplitude, int amplitudeChange, HeightmapGenerator::faultFunctions_t function, int transitionLength)
{
	if (m_connected == false)
	{
		connect();
	}

	m_generateHeightmapFuture = QtConcurrent::run([width, height, iterations, startAmplitude, endAmplitude, amplitudeChange, function, transitionLength]()
	{
		return HeightmapGenerator::generateFault(
			width,
			height,
			iterations,
			startAmplitude,
			endAmplitude,
			amplitudeChange,
			function,
			transitionLength);
	});

	m_generateHeightmapFutureWatcher.setFuture(m_generateHeightmapFuture);
}

void ConcurrencyHandler::onCreateMesh(const Heightmap& heightmap)
{
	if (m_connected == false)
	{
		connect();
	}

	m_createMeshFuture = QtConcurrent::run([heightmap]()
	{
		return Mesh::get(heightmap, 1.0f, 1);
	});

	m_createMeshFutureWatcher.setFuture(m_createMeshFuture);
}

void ConcurrencyHandler::onLoadMesh(const std::string& filename)
{
	if (m_connected == false)
	{
		connect();
	}

	m_loadMeshFuture = QtConcurrent::run([filename]()
	{
		return AssimpIO::loadModel(filename);
	});

	m_loadMeshFutureWatcher.setFuture(m_loadMeshFuture);
}

void ConcurrencyHandler::connect()
{
	QObject::connect(&m_generateHeightmapFutureWatcher, &QFutureWatcher<Heightmap>::finished, this, &ConcurrencyHandler::onHeightmapGenerated);
	QObject::connect(&m_createMeshFutureWatcher, &QFutureWatcher<Mesh>::finished, this, &ConcurrencyHandler::onMeshCreated);
	QObject::connect(&m_loadMeshFutureWatcher, &QFutureWatcher<bool>::finished, this, &ConcurrencyHandler::onMeshLoaded);
	QObject::connect(&m_saveMeshFutureWatcher, &QFutureWatcher<bool>::finished, this, &ConcurrencyHandler::onMeshSaved);

	m_connected = true;
}

void ConcurrencyHandler::onSaveMesh(const Mesh& mesh, const std::string& filename, const std::string& exportFormatId)
{
	if (m_connected == false)
	{
		connect();
	}

	m_saveMeshFuture = QtConcurrent::run([mesh, filename, exportFormatId]()
	{
		return AssimpIO::saveMesh(mesh, filename, exportFormatId);
	});

	m_saveMeshFutureWatcher.setFuture(m_saveMeshFuture);
}

void ConcurrencyHandler::onHeightmapGenerated()
{
	emit heightmapGenerated(m_generateHeightmapFuture.result());
}

void ConcurrencyHandler::onMeshSaved()
{
	emit meshSaved(m_saveMeshFuture.result());
}

void ConcurrencyHandler::onMeshCreated()
{
	emit meshCreated(m_createMeshFuture.result());
}

void ConcurrencyHandler::onMeshLoaded()
{
	Mesh mesh;
	
	if (!m_loadMeshFuture.result().empty())
	{
		mesh = m_loadMeshFuture.result()[0];
	}

	emit meshLoaded(mesh);
}
