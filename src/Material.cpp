
#include "Material.h"

#include <algorithm>

std::unordered_map<GLuint, int> LayeredMaterial::m_textureInstances;

MaterialLayer::MaterialLayer(const Material& material, float maxY, bool textureEnabled)
	: m_material(material)
	, m_maxY(maxY)
	, m_textureEnabled(textureEnabled)
{

}

bool operator<(const MaterialLayer& materialLayer1, const MaterialLayer& materialLayer2) 
{
	return materialLayer1.m_maxY < materialLayer2.m_maxY;
}

LayeredMaterial::LayeredMaterial(const LayeredMaterial& other)
{
	m_materialLayers = other.m_materialLayers;
    m_textureWidth = other.m_textureWidth;
	m_textureHeight = other.m_textureHeight;
	m_textureID = other.m_textureID;

	for (int i = 0; i < m_materialLayers.size(); ++i)
	{
		const MaterialLayer& materialLayer = m_materialLayers[i];
		m_textureLayers[&materialLayer] = other.m_textureLayers.at(&other.m_materialLayers[i]);
		m_hasTexture[&materialLayer] = other.m_hasTexture.at(&other.m_materialLayers[i]);
	}

	if (glIsTexture(m_textureID))
	{
		m_textureInstances[m_textureID]++;
	}
}

LayeredMaterial::LayeredMaterial(LayeredMaterial&& other)
{
	m_materialLayers = std::move(other.m_materialLayers);
	m_textureLayers = std::move(other.m_textureLayers);
	m_hasTexture = std::move(other.m_hasTexture);
	m_textureWidth = other.m_textureWidth;
	m_textureHeight = other.m_textureHeight;
	m_textureID = other.m_textureID;

	other.m_materialLayers.clear();
	other.m_textureLayers.clear();
	other.m_hasTexture.clear();
	other.m_textureID = 0;
	other.m_textureWidth = 0;
	other.m_textureHeight = 0;
}

LayeredMaterial& LayeredMaterial::operator=(const LayeredMaterial& other)
{
	if (&other == this)
	{
		return *this;
	}

	deleteTexture();

	m_materialLayers = other.m_materialLayers;
	m_textureWidth = other.m_textureWidth;
	m_textureHeight = other.m_textureHeight;
	m_textureID = other.m_textureID;

	for (int i = 0; i < m_materialLayers.size(); ++i)
	{
		const MaterialLayer& materialLayer = m_materialLayers[i];
		m_textureLayers[&materialLayer] = other.m_textureLayers.at(&other.m_materialLayers[i]);
		m_hasTexture[&materialLayer] = other.m_hasTexture.at(&other.m_materialLayers[i]);
	}

	if (glIsTexture(m_textureID))
	{
		m_textureInstances[m_textureID]++;
	}

	return *this;
}

LayeredMaterial& LayeredMaterial::operator=(LayeredMaterial&& other)
{
	if (&other == this)
	{
		return *this;
	}

	deleteTexture();

	m_materialLayers = std::move(other.m_materialLayers);
	m_textureLayers = std::move(other.m_textureLayers);
	m_hasTexture = std::move(other.m_hasTexture);
	m_textureWidth = other.m_textureWidth;
	m_textureHeight = other.m_textureHeight;
	m_textureID = other.m_textureID;

	other.m_materialLayers.clear();
	other.m_textureLayers.clear();
	other.m_hasTexture.clear();
	other.m_textureID = 0;
	other.m_textureWidth = 0;
	other.m_textureHeight = 0;

	return *this;
}

LayeredMaterial::~LayeredMaterial()
{
	deleteTexture();
}

void LayeredMaterial::init(const std::vector<MaterialLayer>& materialLayers)
{
	m_materialLayers = materialLayers;
	std::sort(m_materialLayers.begin(), m_materialLayers.end());

	deleteTexture();
	m_textureLayers.clear();
	m_hasTexture.clear();
	for (int i = 0; i < m_materialLayers.size(); ++i)
	{
		const MaterialLayer& materialLayer = m_materialLayers[i];
		m_textureLayers[&materialLayer] = i;
		m_hasTexture[&materialLayer] = false;
	}
}

void LayeredMaterial::init(const std::vector<MaterialLayer>& materialLayers, GLsizei textureWidth, GLsizei textureHeight, GLenum textureFormat, GLenum textureType, const std::vector<const void*>& pixels)
{
	m_materialLayers = materialLayers;
	std::sort(m_materialLayers.begin(), m_materialLayers.end());

	createTexture(textureWidth, textureHeight);

	m_textureLayers.clear();
	m_hasTexture.clear();
	for (int i = 0; i < m_materialLayers.size(); ++i)
	{
		const MaterialLayer& materialLayer = m_materialLayers[i];
		m_textureLayers[&materialLayer] = i;
		m_hasTexture[&materialLayer] = pixels[i];
		if (pixels[i])
		{
			glTextureSubImage3D(m_textureID, 0, 0, 0, i, textureWidth, textureHeight, 1, textureFormat, textureType, pixels[i]);
		}

	}
}

void LayeredMaterial::setMaterialLayer(int layerIndex, const MaterialLayer& materialLayer)
{
	m_materialLayers[layerIndex] = materialLayer;
	std::sort(m_materialLayers.begin(), m_materialLayers.end());
}

void LayeredMaterial::setTextureLayer(int layerIndex, GLenum textureFormat, GLenum textureType, const void* pixels)
{
	const MaterialLayer& materialLayer = m_materialLayers[layerIndex];
	m_hasTexture[&materialLayer] = pixels;

	if (pixels)
	{
		glTextureSubImage3D(m_textureID, 0, 0, 0, m_textureLayers[&materialLayer], m_textureWidth, m_textureHeight, 1, textureFormat, textureType, pixels);
	}
}

const MaterialLayer& LayeredMaterial::getMaterialLayer(int index) const
{
	return m_materialLayers[index];
}

bool LayeredMaterial::hasTexture(int layerIndex) const
{
	return m_hasTexture.at(&m_materialLayers[layerIndex]);
}

const std::vector<MaterialLayer>& LayeredMaterial::getMaterialLayers() const
{
	return m_materialLayers;
}

GLuint LayeredMaterial::getTextureID() const
{
	return m_textureID;
}

int LayeredMaterial::getTextureWidth() const
{
	return m_textureWidth;
}

int LayeredMaterial::getTextureHeight() const
{
	return m_textureHeight;
}

int LayeredMaterial::getLayerCount() const
{
	return m_materialLayers.size();
}

void LayeredMaterial::createTexture(GLsizei width, GLsizei height)
{
	deleteTexture();

	int layerCount = m_materialLayers.size();

	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &m_textureID);
	glTextureStorage3D(m_textureID, 1, GL_RGBA8, width, height, layerCount);

	m_textureInstances[m_textureID] = 1;

	m_textureWidth = width;
	m_textureHeight = height;
}

void LayeredMaterial::deleteTexture()
{
	if (glIsTexture(m_textureID))
	{
		m_textureInstances[m_textureID]--;
		if (m_textureInstances[m_textureID] == 0)
		{
			glDeleteTextures(1, &m_textureID);
		}
		m_textureID = 0;
	}

	m_textureWidth = 0;
	m_textureHeight = 0;
}
