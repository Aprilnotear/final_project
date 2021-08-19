#pragma once
#include "../nclgl/Shader.h"

#include <../nclgl/OGLRenderer.h>
#include <assimp/Scene.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;

struct Vertex {
	Vector3 position;
	Vector3 normal;
	Vector2 texCoord;
	Vector3 tangent;
	Vector3 bitangent;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

class MyMesh {
public:
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	GLuint VAO;

	MyMesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

	void Draw(Shader* shader);

private:
	GLuint VBO, EBO;

	void setupMesh();
};