#pragma once

#include "../nclgl/OGLRenderer.h"
#include "Model.h"

class Camera;
class Mesh;
class HeightMap;

const float LIGHT_MIN_BOUND_D[3] = { -155.0f, -20.0f, -110.0f };
const float LIGHT_MAX_BOUND_D[3] = { 155.0f, 170.0f, 110.0f };

class Deferred : public OGLRenderer {
public:
	Deferred(Window& parent);
	~Deferred(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void FillBuffers();
	void DrawPointLights();
	void CombineBuffers();
	void GenerateScreenTexture(GLuint& into, bool depth = false);
	Vector3 RandomPosition();

	Shader* sceneShader;
	Shader* pointlightShader;
	Shader* combineShader;

	GLuint bufferFBO;
	GLuint bufferColourTex;
	GLuint bufferNormalTex;
	GLuint bufferDepthTex;
	GLuint bufferSpecTex;

	GLuint pointLightFBO;
	GLuint lightDiffuseTex;
	GLuint lightSpecularTex;

	HeightMap* heightMap;
	Light* pointLights;
	Mesh* sphere;
	Mesh* quad;
	Mesh* plane;
	Camera* camera;
	GLuint earthTex;
	GLuint earthBump;

	Model* model;

	vector<Vector3> spherePos;
};
