#pragma once

#include "Mesh.h"
#include <vector>
#include <string>

#include <assimp/scene.h>
#include <assimp/material.h>

using std::string;
using std::vector;

class Model
{
public:
	Model(const char* path)
	{
		loadModel(path);
	}
	void Draw(Shader& shader);
private:
	// model data
	vector<Mesh> meshes;
	string directory;
	vector<Texture> textures_loaded;

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	unsigned int TextureFromFile(const string& path, const string& directory);
};

