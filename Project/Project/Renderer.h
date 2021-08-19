#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/ComputeShader.h"
#include "Model.h"
#include <random>

enum Mode {DEFAULT,DEPTH,LIGHT};

const Vector2 screenSize(1920, 1080);
const float LIGHT_MIN_BOUND[3] = { -135.0f, -20.0f, -60.0f };
const float LIGHT_MAX_BOUND[3] = { 135.0f, 170.0f, 60.0f };

struct LightIndex
{
	int index;
};

struct PointLight {
	Vector4 position;
	Vector4 colour;
	Vector4 paddingAndRadius;
};


class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer(void);

	virtual void RenderScene();
	virtual void UpdateScene(float dt);

protected:
	Model*   model;
	Shader* depthShader;
	Shader* depthDebugShader;
	Shader* lightDebugShader;
	Shader* lightShader;
	ComputeShader* lightCullingShader;
	Camera* camera;
	PointLight* pointLights;
	GLuint depthFBO;
	GLuint depth;
	GLuint workGroup_X;
	GLuint workGroup_Y;
	GLuint lightBuffer;
	GLuint lightIndexBuffer;
	GLfloat camPosition[3];

	Mode mode = DEFAULT;

	void SetDepthFBO();
	void SetFeatures();
	void SetLights();
	void SetShaders();
	Vector3 RandomPosition(uniform_real_distribution<>&dis, mt19937& gen);
};