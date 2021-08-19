#version 430

in VERTEX{
	vec2 texCoords;
	mat3 TBN;
	vec3 ViewPos;
	vec3 FragPos;
} IN;

struct PointLight {
	vec4 position;
	vec4 colour;
	vec4 paddingAndRadius;
};

struct LightIndex {
	int index;
};

// Shader storage buffer objects
layout(std430, binding = 0) readonly buffer LightBuffer{
	PointLight light[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer LightIndicesBuffer{
	LightIndex index[];
} lightIndicesBuffer;

uniform int numberOfTilesX;
uniform int totalLightCount;

out vec4 fragColour;

void main() {
	ivec2 position = ivec2(gl_FragCoord.xy);
	ivec2 groupID = position / ivec2(16, 16);
	uint index = groupID.y * numberOfTilesX + groupID.x;

	uint offset = index * totalLightCount;
	uint i;

	for(i = 0; i< totalLightCount; i++){
	    if(lightIndicesBuffer.index[offset + i].index == -1){
		    break;
		}
	}

	float ratio = float(i) / float(totalLightCount);

	fragColour = vec4(vec3(ratio,ratio,ratio), 1.0);
}
