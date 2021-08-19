#version 330 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1;

in Vertex{
   vec4 colour;
   vec2 texCoord;
   vec3 normal;
   vec3 tangent;
   vec3 binormal;
   vec3 worldPos;
} IN;

out vec4 fragColour[3];

void main(void){
   mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

   vec3 normal = texture2D(texture_normal1, IN.texCoord).rgb * 2.0 - 1.0;
   normal = normalize(TBN * normalize(normal));

   fragColour[0] = texture(texture_diffuse1,IN.texCoord);
   fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
   fragColour[2] = texture(texture_specular1,IN.texCoord);
}
