#version 330 core
out vec4 FragColor;

in vec3 normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = vec4(FragPos, 1.0f);
    //FragColor = texture(texture_diffuse1, TexCoords);
}
