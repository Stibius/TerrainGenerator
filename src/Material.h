#pragma once

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>

class Material
{

public:

	glm::vec4 m_ambient = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
	glm::vec4 m_diffuse = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	glm::vec4 m_specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	int m_shininess = 32;
	glm::vec3 m_textureScale = glm::vec3(1.0f, 1.0f, 1.0f);
};

class MaterialLayer
{

public:

	Material m_material;
	float m_maxY = 1.0f; //!<elevation where this material layer ends
	bool m_textureEnabled = true;

	MaterialLayer() = default;

	explicit MaterialLayer(const Material& material, float maxY = 1.0f, bool textureEnabled = true);

	MaterialLayer(const MaterialLayer& other) = default;

	MaterialLayer(MaterialLayer&& other) = default;

	MaterialLayer& operator=(const MaterialLayer& other) = default;

	MaterialLayer& operator=(MaterialLayer&& other) = default;

	~MaterialLayer() = default;
};

bool operator<(const MaterialLayer& materialLayer1, const MaterialLayer& materialLayer2);

class LayeredMaterial
{

public:

	LayeredMaterial() = default;

	LayeredMaterial(const LayeredMaterial& other);

	LayeredMaterial(LayeredMaterial&& other);

	LayeredMaterial& operator=(const LayeredMaterial& other);

	LayeredMaterial& operator=(LayeredMaterial&& other);

	~LayeredMaterial();

	void init(const std::vector<MaterialLayer>& materialLayers);

	void init(const std::vector<MaterialLayer>& materialLayers, GLsizei textureWidth, GLsizei textureHeight, GLenum textureFormat, GLenum textureType, const std::vector<const void*>& pixels);

	void setMaterialLayer(int layerIndex, const MaterialLayer& materialLayer);

	void setTextureLayer(int layerIndex, GLenum textureFormat, GLenum textureType, const void* pixels);

	const MaterialLayer& getMaterialLayer(int layerIndex) const;

	const std::vector<MaterialLayer>& getMaterialLayers() const;

	bool hasTexture(int layerIndex) const;

	GLuint getTextureID() const;

	int getTextureWidth() const;

	int getTextureHeight() const;

	int getLayerCount() const;

private:

	std::vector<MaterialLayer> m_materialLayers;
	std::unordered_map<const MaterialLayer*, int> m_textureLayers; //!<indices into the array texture for each layer
	std::unordered_map<const MaterialLayer*, bool> m_hasTexture;
	GLuint m_textureID = 0;
	GLsizei m_textureWidth = 0;
	GLsizei m_textureHeight = 0;

	static std::unordered_map<GLuint, int> m_textureInstances; //!<number of array texture instances for each array texture ID

	void createTexture(GLsizei width, GLsizei height);

	void deleteTexture();
};
