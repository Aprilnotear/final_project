#pragma once
#include "Mymesh.h"

#include <../nclgl/OGLRenderer.h>
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

GLuint TextureFromFile(const char* path, const string &directory, bool gamma = false);


class Model {
public:
	vector<Texture> textures_loaded;
	vector<MyMesh> meshes;
	string directory;
	bool gammaCorrection;

	Model(string const& path, bool gamma = false) : gammaCorrection(gamma) {
		this->loadModel(path);
	}

	void Draw(Shader* shader);

private:
	// Loads a model with supported ASSIMP extensions from a file and stores the resulting meshes in the meshes vector.
	void loadModel(const string &path);

	void processNode(aiNode* node, const aiScene* scene);

	MyMesh processMesh(aiMesh* mesh, const aiScene* scene);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet
	vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};