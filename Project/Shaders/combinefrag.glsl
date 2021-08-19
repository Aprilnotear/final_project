#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D specularTex;

in Vertex{
   vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
   vec4 diffuse = texture(diffuseTex, IN.texCoord);
   vec3 light = texture(diffuseLight, IN.texCoord).xyz;
   vec3 specular = texture(specularLight, IN.texCoord).xyz;
   vec3 spec = texture(specularTex, IN.texCoord).xyz;
   float atten = texture(specularLight,IN.texCoord).w;


   fragColour.xyz = diffuse.xyz * 0.1;
   fragColour.xyz += diffuse.xyz * light;
   fragColour.xyz += specular * spec;
   fragColour.xyz += diffuse.xyz * 0.08;

   if(diffuse.a <= 0.2){
       discard;
   }
   fragColour.a = 1.0;
}
