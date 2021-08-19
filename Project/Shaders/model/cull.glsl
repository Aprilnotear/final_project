#version 430

struct PointLight {
	vec4 position;
	vec4 colour;
	vec4 paddingAndRadius;
};

struct LightIndex {
	int index;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight light[];
} lightBuffer;

layout(std430, binding = 1) writeonly buffer LightIndicesBuffer {
	LightIndex index[];
} lightIndicesBuffer;

uniform sampler2D depthMap;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform int lightCount;

shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightNumber;
shared vec4 frustumPlanes[6];
shared int lightIndices[1024];
shared mat4 vpMatrix;



#define TILE_SIZE 8
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main() {
    ivec2 screenSize = ivec2(1920, 1080);
	ivec2 position = ivec2(gl_GlobalInvocationID.xy);
	ivec2 groupID = ivec2(gl_WorkGroupID.xy);
	ivec2 tilesNumber = ivec2(gl_NumWorkGroups.xy);
	uint index = groupID.y * tilesNumber.x + groupID.x;

	barrier();

	vec2 texCoord = vec2(position) / screenSize;
	float depth = texture(depthMap, texCoord).r;

	minDepthInt = 0xFFFFFFFF;
	maxDepthInt = 0;
	visibleLightNumber = 0;
	vpMatrix = projMatrix * viewMatrix;

	uint depthInt = floatBitsToUint(depth);
	atomicMin(minDepthInt, depthInt);
	atomicMax(maxDepthInt, depthInt);

	barrier();

	if (gl_LocalInvocationIndex == 0) {
        //create frustum
		float minGroupDepth = uintBitsToFloat(minDepthInt);
		float maxGroupDepth = uintBitsToFloat(maxDepthInt);

		
		vec2 negaBias = (2.0 * vec2(groupID)) / vec2(tilesNumber);
		vec2 posiBias = (2.0 * vec2(groupID + ivec2(1, 1))) / vec2(tilesNumber);

		frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negaBias.x); // Left
		frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + posiBias.x); // Right
		frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negaBias.y); // Bottom
		frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + posiBias.y); // Top
		frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minGroupDepth); // Near
		frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxGroupDepth); // Far

		for (uint i = 0; i < 6; i++) {
			frustumPlanes[i] *= vpMatrix;
			frustumPlanes[i] /= length(frustumPlanes[i].xyz);
		}
	}
	

	barrier();

    //light culling
	
	uint threadCount = TILE_SIZE * TILE_SIZE;
	uint passCount = (lightCount + threadCount - 1) / threadCount;
	for (uint i = 0; i < passCount; i++) {
		uint lightIndex = i * threadCount + gl_LocalInvocationIndex;

		lightIndex = min(lightIndex, lightCount);
		
		vec4 position = lightBuffer.light[lightIndex].position;
		float radius = lightBuffer.light[lightIndex].paddingAndRadius.w;

		float distance = 0.0;

		
		distance = dot(position, frustumPlanes[5])+ radius;

		if(distance <= 0.0){
		    continue;
		}
		
		
		for (uint j = 0; j < 4; j++) {
			distance = dot(position, frustumPlanes[j]) + radius;

			if (distance <= 0.0) {
				break;
			}
		}
		

		
		if (distance > 0.0) {
			uint offset = atomicAdd(visibleLightNumber, 1);
			lightIndices[offset] = int(lightIndex);
		}

	}
	


	barrier();

	if (gl_LocalInvocationIndex == 0) {
		uint offset = index * lightCount; 
		for (uint i = 0; i < visibleLightNumber; i++) {
			lightIndicesBuffer.index[offset + i].index = lightIndices[i];
		}

		if (visibleLightNumber != lightCount) {
			lightIndicesBuffer.index[offset + visibleLightNumber].index = -1;
		}
	}
}