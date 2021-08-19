#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

out VERTEX{
	vec2 texCoords;
	mat3 TBN;
	vec3 ViewPos;
	vec3 FragPos;
} OUT;

// Uniforms
uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 cameraPos;


void main() {
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
	vec3 fragPosition = vec3(modelMatrix * vec4(position, 1.0));
	OUT.texCoords = texCoord;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 wTangent = normalize(normalMatrix * tangent);
	vec3 wNormal = normalize(normalMatrix * normal);
	vec3 wBitangent = normalize(normalMatrix * bitangent);

	// For tangent space normal mapping
	mat3 TBN = transpose(mat3(wTangent, wBitangent, wNormal));
	OUT.ViewPos = TBN * cameraPos;
	OUT.FragPos = TBN * fragPosition;
	OUT.TBN = TBN;
}
