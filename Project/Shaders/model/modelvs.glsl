#version 330 core
in vec3 aPos;
in vec3 aNormal;
in vec2 aTexCoords;

out vec3 normal;
out vec3 FragPos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    normal = normalize(normalMatrix * normalize(aNormal));
    vec4 worldPos = modelMatrix * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
}


/*
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
}
*/
