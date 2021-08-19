#version 330 core

uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform vec3 cameraPos;

in Vertex{
   vec4 colour;
   vec2 texCoord;
   vec3 normal;
   vec3 worldPos;
} IN;

out vec4 fragColour;

void main(void){
   vec4 diffuse = texture(diffuseTex, IN.texCoord);

   fragColour = (diffuse * 0.25f);
}
