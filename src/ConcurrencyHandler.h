#pragma once

#include <QObject>
#include <QtConcurrent>

#include "AssimpIO.h"
#include "HeightmapGenerator.h"

class ConcurrencyHandler : public QObject
{

	Q_OBJECT

public:

	explicit ConcurrencyHandler(QObject* parent = nullptr);

	ConcurrencyHandler(const ConcurrencyHandler& other) = default;

	ConcurrencyHandler(ConcurrencyHandler&& other) = default;

	ConcurrencyHandler& operator=(const ConcurrencyHandler& other) = default;

	ConcurrencyHandler& operator=(ConcurrencyHandler&& other) = default;

	~ConcurrencyHandler() override = default;

	bool isGeneratingHeightmap() const;

	bool isCreatingMesh() const;

	bool isLoadingMesh() const;

	bool isSavingMesh() const;

public slots:

	void onGenerateHeightmapDiamond(int iterations, int startAmplitude, float amplitudeModifier);

	void onGenerateHeightmapCircles(int width, int height, int minRadius, int maxRadius, int minAmplitude, int maxAmplitude, int iterations);

	void onGenerateHeightmapPerlin(int size, int octaves, float amplitudeModifier, float frequencyModifier);

	void onGenerateHeightmapFault(int width, int height, int iterations, int startAmplitude, int endAmplitude, int amplitudeChange, HeightmapGenerator::faultFunctions_t function, int transitionLength);

	void onCreateMesh(const Heightmap& heightmap);
	
	void onLoadMesh(const std::string& filename);

	void onSaveMesh(const Mesh& mesh, const std::string& filename, const std::string& exportFormatId);

private slots:

	void onHeightmapGenerated();

	void onMeshCreated();

	void onMeshLoaded();

	void onMeshSaved();

signals:

	void heightmapGenerated(const Heightmap& heightmap);

	void meshCreated(const Mesh& mesh);

	void meshLoaded(const Mesh& mesh);

	void meshSaved(bool success);

private:

	void connect();

	bool m_connected = false;

	QFuture<Heightmap> m_generateHeightmapFuture;
	QFutureWatcher<Heightmap> m_generateHeightmapFutureWatcher;
	QFuture<Mesh> m_createMeshFuture;
	QFutureWatcher<Mesh> m_createMeshFutureWatcher;
	QFuture<std::vector<Mesh>> m_loadMeshFuture;
	QFutureWatcher<std::vector<Mesh>> m_loadMeshFutureWatcher;
	QFuture<bool> m_saveMeshFuture;
	QFutureWatcher<bool> m_saveMeshFutureWatcher;
};
