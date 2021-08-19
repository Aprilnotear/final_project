#version 430

struct Light{
    vec4 position;
    vec4 colour;
    float radius;
};

struct LightIndex{
    uint index;
};

layout(std430, binding = 0) buffer LightBuffer{
    Light pointLights[];
}lightBuffer;

layout(std430, binding = 1) buffer LightIndexBuffer{
    LightIndex lightIndex[];
}lightIndexBuffer;

uniform sampler2D depthMap;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform int totalLights;

shared uint minDepth;
shared uint maxDepth;
shared uint visableLightNumber;
shared vec4 frustumPlane[6];

shared uint visableLightIndex[1024];

#define WORK_GROUP_SIZE 16
#define SCREEN_SIZE_X 1920
#define SCREEN_SIZE_Y 1080

layout(local_size_x = WORK_GROUP_SIZE, local_size_y = WORK_GROUP_SIZE, local_size_z = 1) in;

void main(){
    uint index = gl_WorkGroupID.x + gl_WorkGroupID.y * gl_NumWorkGroups.x;

    if(gl_LocalInvocationIndex == 0){
        minDepth = 0xFFFFFFFF;
        maxDepth = 0;
        visableLightNumber = 0;
    }

    barrier();

    vec2 text = vec2(gl_GlobalInvocationID.xy) / vec2(SCREEN_SIZE_X,SCREEN_SIZE_Y);
    float depth = texture(depthMap, text).r;

    uint depthUI = uint(depth);
    atomicMin(minDepth,depthUI);
    atomicMax(maxDepth,depthUI);

    barrier();

    if(gl_LocalInvocationIndex == 0){
        float minDepthF = float(minDepth);
        float maxDepthF = float(maxDepth);

        vec2 tileScale = vec2(SCREEN_SIZE_X,SCREEN_SIZE_Y) / float (2 * WORK_GROUP_SIZE);
        vec2 tileBias = tileScale - vec2(gl_WorkGroupID.xy);

        vec4 c1 = vec4(projMatrix[0][0] * tileScale.x, 0.0f, tileBias.x, 0.0f);
        vec4 c2 = vec4(0.0f, -projMatrix[1][1] * tileScale.y, tileBias.y, 0.0f);
        vec4 c4 = vec4(0.0f,0.0f,1.0f,0.0f);

        frustumPlane[0] = c4 - c1;
        frustumPlane[1] = c1;
        frustumPlane[2] = c4 - c2;
        frustumPlane[3] = c2;
        frustumPlane[4] = vec4(0.0,0.0,-1.0,-minDepthF);
        frustumPlane[5] = vec4(0.0,0.0,1.0,maxDepthF);

        for(int i=0; i<4; ++i){
            frustumPlane[i] /= length(frustumPlane[i].xyz);
        }
    }


    barrier();

    uint totalWorkGroups = WORK_GROUP_SIZE * WORK_GROUP_SIZE;
    uint passCount = (totalLights + totalWorkGroups - 1) / totalWorkGroups;
    for(uint i = 0; i< passCount; ++i){
        uint lightIndex = i * totalWorkGroups + gl_LocalInvocationIndex;

        lightIndex = min(lightIndex, totalLights);

        vec3 p = lightBuffer.pointLights[lightIndex].position.xyz;
        vec4 position = viewMatrix * vec4(p,1.0);
        float r = lightBuffer.pointLights[lightIndex].radius;

        bool inFrutum = true;
        for(uint j=0; j<6 && inFrutum; ++j){
            float d = dot(frustumPlane[j], position) + r;
            inFrutum = (d > 0);
        }
        if(inFrutum){
            uint id = atomicAdd(visableLightNumber, 1);
            visableLightIndex[id] = lightIndex;
        }
    }

    barrier();

    if(gl_LocalInvocationIndex == 0){
        uint offset = index * 1024;
        for(uint i = 0; i<visableLightNumber; ++i){
            lightIndexBuffer.lightIndex[offset + i].index = visableLightIndex[i];
        }

        if(visableLightNumber != 1024){
            lightIndexBuffer.lightIndex[offset + visableLightNumber].index = -1;
        }
    }

}
