#include "Renderer.h"
#include "../nclgl/Light.h"

const GLint LIGHT_NUM = 1024;
const float nearPlane = 0.1f;
const float farPlane = 300.0f;
const int TILES_NUM = 16;

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	SetFeatures();

	depthShader = new Shader("/model/depthvert.glsl", "/model/depthfrag.glsl");
	depthDebugShader = new Shader("/model/depth_Debug_vs.glsl", "/model/depth_Debug_fs.glsl");
	lightDebugShader = new Shader("/model/light_Debug_vs.glsl", "/model/light_Debug_fs.glsl");
	lightCullingShader = new ComputeShader("/model/cull.glsl");
	lightShader = new Shader("/model/light_Debug_vs.glsl", "/model/light_fs.glsl");
	

	
	if (!depthShader->LoadSuccess() || !depthDebugShader->LoadSuccess() || !lightDebugShader->LoadSuccess() ||!lightShader->LoadSuccess()) {
		return;
	}

	camera = new Camera(0.0f, 0.0f, Vector3(-40.0f, 10.0f, 0.0f));
	
	SetDepthFBO();

	SetLights();
	model = new Model(MESHDIR"model/crytek-sponza/sponza.obj");
	SetShaders();

	glViewport(0, 0, (int)screenSize.x, (int)screenSize.y);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	init = true;
}

Renderer::~Renderer(void) {
	delete depthShader;
	delete depthDebugShader;
	delete lightDebugShader;
	delete lightCullingShader;
	delete lightShader;
	delete model;
	delete camera;
}

void Renderer::RenderScene() {
	projMatrix = Matrix4::Perspective(nearPlane, farPlane, (float)width / (float)height, 45.0f);
	viewMatrix = camera->BuildViewMatrix();

	BindShader(depthShader);
	glUniformMatrix4fv(glGetUniformLocation(depthShader->GetProgram(), "projection"), 1, GL_FALSE, projMatrix.values);
	glUniformMatrix4fv(glGetUniformLocation(depthShader->GetProgram(), "view"), 1, GL_FALSE, viewMatrix.values);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	model->Draw(depthShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	lightCullingShader->Bind();
	glUniformMatrix4fv(glGetUniformLocation(lightCullingShader->GetProgram(), "projMatrix"), 1, GL_FALSE, projMatrix.values);//
	glUniformMatrix4fv(glGetUniformLocation(lightCullingShader->GetProgram(), "viewMatrix"), 1, GL_FALSE, viewMatrix.values);

	glActiveTexture(GL_TEXTURE4);
	glUniform1i(glGetUniformLocation(lightCullingShader->GetProgram(), "depthMap"), 4);
	glBindTexture(GL_TEXTURE_2D, depth);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightIndexBuffer);

	glDispatchCompute(workGroup_X, workGroup_Y, 1);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, 0);


	if (mode == DEFAULT) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		BindShader(lightShader);
		glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "projMatrix"), 1, GL_FALSE, projMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "viewMatrix"), 1, GL_FALSE, viewMatrix.values);
		glUniform3fv(glGetUniformLocation(lightShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

		model->Draw(lightShader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else if (mode == DEPTH) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		BindShader(depthDebugShader);
		UpdateShaderMatrices();
		model->Draw(depthDebugShader);
	}
	else if (mode == LIGHT) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		BindShader(lightDebugShader);
		glUniformMatrix4fv(glGetUniformLocation(lightDebugShader->GetProgram(), "viewMatrix"), 1, GL_FALSE, viewMatrix.values);
		glUniformMatrix4fv(glGetUniformLocation(lightDebugShader->GetProgram(), "projMatrix"), 1, GL_FALSE, projMatrix.values);
		glUniform3fv(glGetUniformLocation(lightDebugShader->GetProgram(), "cameraPos"), 1, (float*)&camera->GetPosition());

		model->Draw(lightDebugShader);

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);

	}

}

void Renderer::UpdateScene(float dt) {
	camera->UpdateCamera(dt);
}

void Renderer::SetFeatures() {
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
}

Vector3 Renderer::RandomPosition(uniform_real_distribution<>&dis, mt19937& gen) {
	Vector3 position;
	float li[3];
	for (int i = 0; i < 3; ++i) {
		float min = LIGHT_MIN_BOUND[i];
		float max = LIGHT_MAX_BOUND[i];

		float value = (float)dis(gen) * (max - min) + min;
		li[i] = value;
	}
	position.x = li[0];
	position.y = li[1];
	position.z = li[2];
	return position;
}

void Renderer::SetLights() {
	workGroup_X = ((int)screenSize.x + TILES_NUM - 1) / TILES_NUM;
	workGroup_Y = ((int)screenSize.y + TILES_NUM - 1) / TILES_NUM;
	GLuint workGroups = workGroup_X * workGroup_Y;

	glGenBuffers(1, &lightBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, LIGHT_NUM * sizeof(PointLight), 0, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &lightIndexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightIndexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, workGroups * sizeof(LightIndex) * 1024, 0, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightBuffer);
	pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<>dis(0, 1);

	for (int i = 0; i < LIGHT_NUM; ++i) {
		PointLight& l = pointLights[i];
		Vector3 lightPos = RandomPosition(dis, gen);
		l.position = Vector4(lightPos.x, lightPos.y, lightPos.z, 1.0f);
		l.colour = (Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX), 1));
		l.paddingAndRadius = Vector4(0, 0, 0, 30.0f);//
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void Renderer::SetDepthFBO() {
	glGenFramebuffers(1, &depthFBO);

	glGenTextures(1, &depth);
	glBindTexture(GL_TEXTURE_2D, depth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenSize.x, screenSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::SetShaders() {
	modelMatrix = Matrix4::Scale(Vector3(0.1f, 0.1f, 0.1f));
	BindShader(depthShader);
	glUniformMatrix4fv(glGetUniformLocation(depthShader->GetProgram(), "modelMatrix"), 1, GL_FALSE, modelMatrix.values);

	lightCullingShader->Bind();
	glUniform1i(glGetUniformLocation(lightCullingShader->GetProgram(), "lightCount"), LIGHT_NUM);
	glUniform2iv(glGetUniformLocation(lightCullingShader->GetProgram(), "screenSize"), 1, (int*)&screenSize);


	if (mode == DEFAULT) {
		BindShader(lightShader);
		glUniformMatrix4fv(glGetUniformLocation(lightShader->GetProgram(), "modelMatrix"), 1, GL_FALSE, modelMatrix.values);
		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "numberOfTilesX"), workGroup_X);
		glUniform1i(glGetUniformLocation(lightShader->GetProgram(), "lightCount"), LIGHT_NUM);
	}
	else if (mode == DEPTH) {
		BindShader(depthDebugShader);
		glUniform1f(glGetUniformLocation(depthDebugShader->GetProgram(), "near"), nearPlane);
		glUniform1f(glGetUniformLocation(depthDebugShader->GetProgram(), "far"), farPlane);
	}
	else if (mode == LIGHT) {
		BindShader(lightDebugShader);
		glUniformMatrix4fv(glGetUniformLocation(lightDebugShader->GetProgram(), "modelMatrix"), 1, GL_FALSE, modelMatrix.values);
		glUniform1i(glGetUniformLocation(lightDebugShader->GetProgram(), "totalLightCount"), LIGHT_NUM);
		glUniform1i(glGetUniformLocation(lightDebugShader->GetProgram(), "numberOfTilesX"), workGroup_X);
	}

}