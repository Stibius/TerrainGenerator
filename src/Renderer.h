#pragma once

#include <GL/glew.h>

#include <string>

#include "DirectionalLight.h"
#include "Material.h"
#include "Mesh.h"
#include "OrbitPerspectiveCamera.h"
#include "FreeLookOrthoCamera.h"
#include "ShaderProgram.h"

class Renderer
{

public:

	static constexpr int MAX_MATERIAL_LAYERS = 14;

	Renderer();
	
	Renderer(const Renderer& other) = delete;

	Renderer(Renderer&& other) = delete;

	Renderer& operator=(const Renderer& other) = delete;

	Renderer& operator=(Renderer&& other) = delete;

	~Renderer();

	void init();

	void setViewportSize(int width, int height);

	void setLight(const DirectionalLight& light);

	void setCamera(const OrbitPerspectiveCamera& camera);

	void setMesh(const Mesh& mesh);

	void setMaterial(const LayeredMaterial& material);

	void setShaders(const std::string& terrainVSSource,
		const std::string& terrainFSSource,
		const std::string& depthMapVSSource,
		const std::string& depthMapFSSource);

	void setPoissonSpread(int poissonSpread);

	void setShadowBias(float shadowBias);

	void enableShadows(bool shadowsEnabled);

	void setMode(GLenum mode) const;

	int render();

	const DirectionalLight& getLight() const;

	const OrbitPerspectiveCamera& getCamera() const;

	const Mesh& getMesh() const;

	const LayeredMaterial& getMaterial() const;

	int getPoissonSpread() const;

	float getShadowBias() const;

	bool shadowsEnabled() const;

	GLint getMode() const;

private:

	static bool m_glewInitialized; 
	bool m_needToCompileShaders = false;
	bool m_needToProcessScene = false;
	bool m_needToUpdateLightUniforms = false;
	bool m_needToUpdateCameraUniforms = false;
	bool m_needToUpdateMaterialUniforms = false;
	bool m_needToUpdateShadowUniforms = false;
	bool m_needToUpdateMeshUniforms = false;

	int m_viewPortWidth;
	int m_viewPortHeight;

	LayeredMaterial m_material;

	OrbitPerspectiveCamera m_camera;

	DirectionalLight m_light;
	FreeLookOrthoCamera m_lightCamera;

	Mesh m_mesh;
	GLuint m_terrainVAO; 
	GLuint m_terrainVBO; 
	GLuint m_terrainEBO;
	int m_terrainIndexCount = 0; 
	std::string m_terrainVSSource;
	std::string m_terrainFSSource;
	ShaderProgram m_terrainProgram;

	int m_poissonSpread = 700;
	float m_shadowBias = 0.001;
	static constexpr GLuint SHADOWMAP_WIDTH = 1024;
	static constexpr GLuint SHADOWMAP_HEIGHT = 1024;
	bool m_shadowsEnabled = true;
	GLuint m_depthMapFBO;
	GLuint m_depthMapTextureID;
	std::string m_depthMapVSSource;
	std::string m_depthMapFSSource;
	ShaderProgram m_depthMapProgram;

	bool setShaders();

	void createDepthMap();

	void processScene();

	void updateLightUniforms();

	void updateCameraUniforms();

	void updateMaterialUniforms();

	void updateShadowUniforms();

	void updateMeshUniforms();

	void updateUniforms();

	void setLightCamera();

	void drawSetup();

	void drawTerrain() const;

	void drawDepthMap() const;

};
