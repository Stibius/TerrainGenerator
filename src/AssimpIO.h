#pragma once

#include <vector>

#include <assimp/scene.h>

#include "Mesh.h"

class AssimpIO
{

public:

	struct ExportFormat
	{
		std::string id;
		std::string description;
		std::string extension;
	};

	static std::vector<Mesh> loadModel(const std::string& filename);

	static bool saveMesh(const Mesh& mesh, const std::string& filename, const std::string& formatId);

	static std::vector<std::string> getImportExtensions();

	static std::vector<ExportFormat> getExportFormats();

private:

	static std::vector<Mesh> processNode(aiNode* node, const aiScene* scene);

	static Mesh processMesh(aiMesh* mesh, const aiScene* scene);

};
