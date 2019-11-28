
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>

#include "AssimpIO.h"

std::vector<Mesh> AssimpIO::loadModel(const std::string& filename)
{
	std::vector<Mesh> meshes;

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
	{
		return meshes;
	}

	meshes = processNode(scene->mRootNode, scene);

	return meshes;
}

bool AssimpIO::saveMesh(const Mesh& mesh, const std::string& filename, const std::string& formatId)
{
	if (mesh.empty())
	{
		return false;
	}
	
	aiScene scene;

	scene.mRootNode = new aiNode();

	scene.mMaterials = new aiMaterial*[1];
	scene.mMaterials[0] = new aiMaterial();
	scene.mNumMaterials = 1;

	scene.mMeshes = new aiMesh*[1];
	scene.mMeshes[0] = new aiMesh();
	scene.mNumMeshes = 1;

	scene.mMeshes[0]->mMaterialIndex = 0;

	scene.mRootNode->mMeshes = new unsigned int[1];
	scene.mRootNode->mMeshes[0] = 0;
	scene.mRootNode->mNumMeshes = 1;

	aiMesh* pMesh = scene.mMeshes[0];

	const Vertex* vVertices = mesh.getVertices().data();

	pMesh->mVertices = new aiVector3D[mesh.getVertices().size()];
	pMesh->mNumVertices = mesh.getVertices().size();

	for (int i = 0; i < mesh.getVertices().size(); ++i)
	{
		glm::vec3 v = vVertices[i].m_position;

		pMesh->mVertices[i] = aiVector3D(v.x, v.y, v.z);
	}

	pMesh->mFaces = new aiFace[mesh.getIndices().size() / 3];
	pMesh->mNumFaces = mesh.getIndices().size() / 3;

	for (int i = 0; i < mesh.getIndices().size() / 3; ++i)
	{
		aiFace& face = pMesh->mFaces[i];

		face.mIndices = new unsigned int[3];
		face.mNumIndices = 3;

		face.mIndices[0] = mesh.getIndices()[i * 3];
		face.mIndices[1] = mesh.getIndices()[i * 3 + 1];
		face.mIndices[2] = mesh.getIndices()[i * 3 + 2];
	}

	Assimp::Exporter exporter;
	aiReturn ret = exporter.Export(&scene, formatId, filename, scene.mFlags);
	
	return ret == aiReturn_SUCCESS;
}

std::vector<std::string> AssimpIO::getImportExtensions()
{
	std::vector<std::string> extensions;

	Assimp::Importer importer;
	std::string szOut;
	importer.GetExtensionList(szOut);

	size_t offset = 0;
	while (offset < szOut.size())
	{
		size_t end = szOut.find(';', offset);

		if (end == std::string::npos)
		{
			std::string extension = szOut.substr(offset);
			extensions.push_back(extension);

			offset = szOut.size();
		}
		else
		{
			std::string extension = szOut.substr(offset, end - offset);
			extensions.push_back(extension);

			offset = end + 1;
		}
	}

	return extensions;
}

std::vector<AssimpIO::ExportFormat> AssimpIO::getExportFormats()
{
	Assimp::Exporter exporter;
	std::vector<ExportFormat> formats;
	formats.reserve(exporter.GetExportFormatCount());
	for (int i = 0; i < exporter.GetExportFormatCount(); ++i)
	{
		const aiExportFormatDesc* aiFormat = exporter.GetExportFormatDescription(i);
		ExportFormat format;
		format.id = aiFormat->id;
		format.description = aiFormat->description;
		format.extension = aiFormat->fileExtension;
		formats.push_back(format);
	}

	return formats;
}

std::vector<Mesh> AssimpIO::processNode(aiNode* node, const aiScene* scene)
{
	std::vector<Mesh> meshes;
	meshes.reserve(node->mNumMeshes);

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		std::vector<Mesh> meshes2 = processNode(node->mChildren[i], scene);
		meshes.insert(meshes.end(), meshes2.begin(), meshes2.end());
	}

	return meshes;
}

Mesh AssimpIO::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	vertices.reserve(mesh->mNumVertices);
	
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;

		vertex.m_position.x = mesh->mVertices[i].x;
		vertex.m_position.y = mesh->mVertices[i].y;
		vertex.m_position.z = mesh->mVertices[i].z;

		vertices.push_back(vertex);
	}

	std::vector<int> indices;
	indices.reserve(3 * mesh->mNumFaces);

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		indices.push_back(mesh->mFaces[i].mIndices[0]);
		indices.push_back(mesh->mFaces[i].mIndices[1]);
		indices.push_back(mesh->mFaces[i].mIndices[2]);
	}

	Mesh mmesh = Mesh(vertices, indices, 1);

	return mmesh;
}
