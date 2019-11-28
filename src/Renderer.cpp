
#include "Renderer.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <utility>

bool Renderer::m_glewInitialized = false;

Renderer::Renderer()
{
	std::vector<MaterialLayer> materialLayers = { MaterialLayer(Material(), 1.0f) };
	m_material.init(materialLayers);
}

Renderer::~Renderer()
{
	if (m_glewInitialized == false) return;

	if (glIsBuffer(m_terrainVBO))
	{
		glDeleteBuffers(1, &m_terrainVBO);
	}

	if (glIsBuffer(m_terrainEBO))
	{
		glDeleteBuffers(1, &m_terrainEBO);
	}

	if (glIsBuffer(m_depthMapFBO))
	{
		glDeleteBuffers(1, &m_depthMapFBO);
	}

	if (glIsVertexArray(m_terrainVAO))
	{
		glDeleteVertexArrays(1, &m_terrainVAO);
	}

	if (glIsTexture(m_depthMapTextureID))
	{
		glDeleteTextures(1, &m_depthMapTextureID);
	}
}

void Renderer::init()
{
	if (m_glewInitialized == false)
	{
		if (glewInit())
		{
			return;
		}

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL); 
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_glewInitialized = true;

		createDepthMap();
	}
}

void Renderer::setViewportSize(int width, int height)
{
	if (m_glewInitialized == false) return;

	if (height == 0)									
	{
		height = 1;									
	}

	m_viewPortWidth = width;
	m_viewPortHeight = height;

	glViewport(0, 0, width, height);					

	m_camera.m_perspectiveCamera.setAspect(static_cast<float>(width) / static_cast<float>(height));

	m_needToUpdateCameraUniforms = true;
}

void Renderer::setLight(const DirectionalLight& light)
{
	m_light = light;

	setLightCamera();

	m_needToUpdateLightUniforms = true;
}

void Renderer::setCamera(const OrbitPerspectiveCamera& camera)
{
	m_camera = camera;

	m_needToUpdateCameraUniforms = true;
}

void Renderer::setMesh(const Mesh& mesh)
{
	m_mesh = mesh;

	setLightCamera();

	m_needToProcessScene = true;
	m_needToUpdateLightUniforms = true;
	m_needToUpdateMeshUniforms = true;
}

void Renderer::setMaterial(const LayeredMaterial& material)
{
	m_material = material;

	m_needToUpdateMaterialUniforms = true;
}

void Renderer::setShaders(const std::string& terrainVSSource,
	const std::string& terrainFSSource,
	const std::string& depthMapVSSource,
	const std::string& depthMapFSSource)
{
	m_terrainVSSource = terrainVSSource;
	m_terrainFSSource = terrainFSSource;
	m_depthMapVSSource = depthMapVSSource;
	m_depthMapFSSource = depthMapFSSource;

	m_needToCompileShaders = true;
	m_needToProcessScene = true;
	m_needToUpdateLightUniforms = true;
	m_needToUpdateCameraUniforms = true;
	m_needToUpdateMaterialUniforms = true;
	m_needToUpdateShadowUniforms = true;
}

void Renderer::setPoissonSpread(int poissonSpread)
{
	if (poissonSpread > 0)
	{
		m_poissonSpread = poissonSpread;

		m_needToUpdateShadowUniforms = true;
	}
}

void Renderer::setShadowBias(float shadowBias)
{
	if (shadowBias >= 0.0f)
	{
		m_shadowBias = shadowBias;

		m_needToUpdateShadowUniforms = true;
	}
}

void Renderer::enableShadows(bool shadowsEnabled)
{
	m_shadowsEnabled = shadowsEnabled;

	m_needToUpdateShadowUniforms = true;
}

void Renderer::setMode(GLenum mode) const
{
	if (m_glewInitialized == false) return;

	glPolygonMode(GL_FRONT_AND_BACK, mode);
}

int Renderer::render() 
{
	if (m_glewInitialized == false) return 0;

	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawSetup();

	if (m_shadowsEnabled)
	{
		drawDepthMap();
		drawTerrain();
	}
	else
	{
		drawTerrain();
	}

	glFinish();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	
	std::chrono::duration<int64_t, std::micro> elapsed = std::chrono::duration_cast<std::chrono::duration<int64_t, std::micro>>(t2 - t1);

	return elapsed.count();
}

const DirectionalLight& Renderer::getLight() const
{
	return m_light;
}

const OrbitPerspectiveCamera& Renderer::getCamera() const
{
	return m_camera;
}

const Mesh& Renderer::getMesh() const
{
	return m_mesh;
}

const LayeredMaterial& Renderer::getMaterial() const
{
	return m_material;
}

int Renderer::getPoissonSpread() const
{
	return m_poissonSpread;
}

float Renderer::getShadowBias() const
{
	return m_shadowBias;
}

bool Renderer::shadowsEnabled() const
{
	return m_shadowsEnabled;
}

GLint Renderer::getMode() const
{
	if (m_glewInitialized == false) return 0;

	GLint polygonMode;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

	return polygonMode;
}

bool Renderer::setShaders()
{
	if (m_glewInitialized == false) return false;

	//Mesh

	Shader terrainVS;
	Shader terrainFS;
	if (!terrainVS.compile(GL_VERTEX_SHADER, m_terrainVSSource))
	{
		std::cout << terrainVS.getErrorMessage() << "\n";
		return false;
	}
	if (!terrainFS.compile(GL_FRAGMENT_SHADER, m_terrainFSSource))
	{
		std::cout << terrainFS.getErrorMessage() << "\n";
		return false;
	}
	m_terrainProgram.linkShaders({ terrainVS, terrainFS });
	if (!m_terrainProgram.isLinked())
	{
		std::cout << m_terrainProgram.getErrorMessage() << "\n";
		return false;
	}

	//Depth Map

	Shader depthMapVS;
	Shader depthMapFS;
	if (!depthMapVS.compile(GL_VERTEX_SHADER, m_depthMapVSSource))
	{
		std::cout << depthMapVS.getErrorMessage() << "\n";
		return false;
	}
	if (!depthMapFS.compile(GL_FRAGMENT_SHADER, m_depthMapFSSource))
	{
		std::cout << depthMapFS.getErrorMessage() << "\n";
		return false;
	}
	m_depthMapProgram.linkShaders({ depthMapVS, depthMapFS });
	if (!m_depthMapProgram.isLinked())
	{
		std::cout << m_depthMapProgram.getErrorMessage() << "\n";
		return false;
	}

	m_needToCompileShaders = false;

	return true;
}

void Renderer::createDepthMap()
{
	if (m_glewInitialized == false) return;

	glCreateTextures(GL_TEXTURE_2D, 1, &m_depthMapTextureID);
	glTextureStorage2D(m_depthMapTextureID, 1, GL_DEPTH_COMPONENT, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	glTextureSubImage2D(m_depthMapTextureID, 0, 0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTextureParameteri(m_depthMapTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(m_depthMapTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(m_depthMapTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_depthMapTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_depthMapTextureID, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glCreateFramebuffers(1, &m_depthMapFBO);
	glNamedFramebufferTexture(m_depthMapFBO, GL_DEPTH_ATTACHMENT, m_depthMapTextureID, 0);
	glNamedFramebufferDrawBuffer(m_depthMapFBO, GL_NONE);
	glNamedFramebufferReadBuffer(m_depthMapFBO, GL_NONE);
	
	m_needToUpdateShadowUniforms = true;
}

void Renderer::processScene()
{
	if (m_glewInitialized == false) return;

	if (glIsBuffer(m_terrainVBO))
	{
		glDeleteBuffers(1, &m_terrainVBO);
	}

	if (glIsBuffer(m_terrainEBO))
	{
		glDeleteBuffers(1, &m_terrainEBO);
	}

	if (glIsVertexArray(m_terrainVAO))
	{
		glDeleteVertexArrays(1, &m_terrainVAO);
	}

	if (m_terrainProgram.isLinked() && !m_mesh.empty())
	{
		const Vertex* vertices = m_mesh.getVertices().data();
		int verticesSize = m_mesh.getVertices().size();
		const int* indices = m_mesh.getIndices().data();
		int indicesSize = m_mesh.getIndices().size();
		m_terrainIndexCount = indicesSize;

		glGenVertexArrays(1, &m_terrainVAO);
		glBindVertexArray(m_terrainVAO);

		glGenBuffers(1, &m_terrainVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);
		glBufferData(GL_ARRAY_BUFFER, verticesSize * sizeof(Vertex), vertices, GL_STATIC_DRAW);

		m_terrainProgram.setVertexAttribPointer("position", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, m_position)));
		m_terrainProgram.setVertexAttribPointer("texCoords", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, m_texCoords)));
		m_terrainProgram.setVertexAttribPointer("normal", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, m_normal)));

		m_depthMapProgram.setVertexAttribPointer("position", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, m_position)));
		m_depthMapProgram.setVertexAttribPointer("normal", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, m_normal)));

		glGenBuffers(1, &m_terrainEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(int), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	m_needToProcessScene = false;
}

void Renderer::updateLightUniforms()
{
	if (m_glewInitialized == false) return;

	if (m_terrainProgram.isLinked())
	{
		m_terrainProgram.use();
		m_terrainProgram.setUniform("light.color", 1, m_light.m_color);
		m_terrainProgram.setUniform("light.direction", 1, m_light.m_direction);
		m_terrainProgram.setUniform("lightVP", 1, GL_FALSE, m_lightCamera.getViewProjectionMatrix());
	}

	if (m_depthMapProgram.isLinked())
	{
		m_depthMapProgram.use();
		m_depthMapProgram.setUniform("vp", 1, GL_FALSE, m_lightCamera.getViewProjectionMatrix());
	}

	m_needToUpdateLightUniforms = false;
}

void Renderer::updateCameraUniforms()
{
	if (m_glewInitialized == false) return;

	if (m_terrainProgram.isLinked())
	{
		m_terrainProgram.use();
		m_terrainProgram.setUniform("cameraVP", 1, GL_FALSE, m_camera.getViewProjectionMatrix());
		m_terrainProgram.setUniform("cameraPos", 1, m_camera.getPosition());
	}
		
	m_needToUpdateCameraUniforms = false;
}

void Renderer::updateMaterialUniforms()
{
	if (m_glewInitialized == false) return;

	if (m_terrainProgram.isLinked())
	{
		m_terrainProgram.use();

		if (glIsTexture(m_material.getTextureID()))
		{
			glBindTextureUnit(0, m_material.getTextureID());
		}
		else
		{
			glBindTextureUnit(0, 0);
		}

		const std::vector<MaterialLayer>& materialLayers = m_material.getMaterialLayers();
		m_terrainProgram.setUniform("materialCount", static_cast<GLint>(materialLayers.size()));
		m_terrainProgram.setUniform("diffuseTex", 0);

		for (int i = 0; i < materialLayers.size(); ++i)
		{
			std::string iStr = std::to_string(i);

			const MaterialLayer& materialLayer = materialLayers[i];

			m_terrainProgram.setUniform("materials[" + iStr + "].maxY", materialLayer.m_maxY);
			m_terrainProgram.setUniform("materials[" + iStr + "].ambient", 1, materialLayer.m_material.m_ambient);
			m_terrainProgram.setUniform("materials[" + iStr + "].diffuse", 1, materialLayer.m_material.m_diffuse);
			m_terrainProgram.setUniform("materials[" + iStr + "].specular", 1, materialLayer.m_material.m_specular);
			m_terrainProgram.setUniform("materials[" + iStr + "].shininess", materialLayer.m_material.m_shininess);
			m_terrainProgram.setUniform("materials[" + iStr + "].textureScale", 1, materialLayer.m_material.m_textureScale);

			if (materialLayer.m_textureEnabled && m_material.hasTexture(i) && glIsTexture(m_material.getTextureID()))
			{
				m_terrainProgram.setUniform("materials[" + iStr + "].hasDiffuseTex", 1);
			}
			else
			{
				m_terrainProgram.setUniform("materials[" + iStr + "].hasDiffuseTex", 0);
			}
		}
	}

	m_needToUpdateMaterialUniforms = false;
}

void Renderer::updateShadowUniforms()
{
	if (m_glewInitialized == false) return;

	if (m_terrainProgram.isLinked())
	{
		m_terrainProgram.use();

		m_terrainProgram.setUniform("shadowMap", 1);
		m_terrainProgram.setUniform("poissonSpread", m_poissonSpread);
		m_terrainProgram.setUniform("shadowBias", m_shadowBias);

		if (m_shadowsEnabled && glIsTexture(m_depthMapTextureID))
		{
			m_terrainProgram.setUniform("shadowsEnabled", 1);
			glBindTextureUnit(1, m_depthMapTextureID);
		}
		else
		{
			m_terrainProgram.setUniform("shadowsEnabled", 0);
			glBindTextureUnit(1, 0);
		}
	}

	m_needToUpdateShadowUniforms = false;
}

void Renderer::updateMeshUniforms()
{
	if (m_glewInitialized == false) return;

	if (m_terrainProgram.isLinked())
	{
		m_terrainProgram.use();
		m_terrainProgram.setUniform("minY", m_mesh.getBoundingBox().m_min.y);
		m_terrainProgram.setUniform("maxY", m_mesh.getBoundingBox().m_max.y);
	}

	m_needToUpdateMeshUniforms = false;
}

void Renderer::updateUniforms()
{
	if (m_needToUpdateLightUniforms == true) 
		updateLightUniforms();

	if (m_needToUpdateCameraUniforms == true)
		updateCameraUniforms();

	if (m_needToUpdateMaterialUniforms == true)
		updateMaterialUniforms();

	if (m_needToUpdateShadowUniforms == true)
		updateShadowUniforms();

	if (m_needToUpdateMeshUniforms == true)
		updateMeshUniforms();
}

void Renderer::setLightCamera()
{
	float radius = sqrt(pow(sqrt(pow(m_mesh.getBoundingBox().m_size.x, 2.0) + pow(m_mesh.getBoundingBox().m_size.y, 2.0)), 2.0) + pow(m_mesh.getBoundingBox().m_size.z, 2.0)) / 2.0;
	glm::vec3 pos = -m_light.m_direction;
	pos = glm::normalize(pos);
	pos *= radius;
	m_lightCamera.m_orthoCamera = OrthoCamera(-radius, radius, -radius, radius, m_lightCamera.m_orthoCamera.getNear(), radius * 10);
	glm::vec3 secondPos = glm::vec3(pos.x + 1, pos.y, pos.z);
	glm::vec4 lightUp = glm::normalize(glm::vec4(glm::cross(glm::vec3(pos), secondPos), 0));
	if (lightUp.y < 0) lightUp *= -1;
	m_lightCamera.m_freeLookCamera.setPosition(pos);
	m_lightCamera.m_freeLookCamera.setRotation(m_light.m_direction, lightUp);

	m_needToUpdateLightUniforms = true;
}

void Renderer::drawSetup()
{
	if (m_needToCompileShaders == true)
	{
		setShaders();
	}

	if (m_needToProcessScene == true)
	{
		processScene();
	}

	updateUniforms();
}

void Renderer::drawTerrain() const
{
	if (m_glewInitialized == false || !m_terrainProgram.isLinked() || !glIsVertexArray(m_terrainVAO))
	{
		return;
	}

	m_terrainProgram.use();

	glBindVertexArray(m_terrainVAO);

	glDrawElements(GL_TRIANGLES, m_terrainIndexCount, GL_UNSIGNED_INT, NULL);
}

void Renderer::drawDepthMap() const
{
	if (m_glewInitialized == false || !m_depthMapProgram.isLinked() || !glIsVertexArray(m_terrainVAO) || !glIsFramebuffer(m_depthMapFBO))
	{
		return;
	}

	m_depthMapProgram.use();

	glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(m_terrainVAO);

	glDrawElements(GL_TRIANGLES, m_terrainIndexCount, GL_UNSIGNED_INT, NULL);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, m_viewPortWidth, m_viewPortHeight);
}
