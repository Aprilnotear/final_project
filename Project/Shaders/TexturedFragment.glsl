#version 150 core
uniform sampler2D diffuseTex;

in Vertex {
   vec2 texCoord;
} IN;


out vec4 fragColour;
void main(void) {
   fragColour = texture(diffuseTex, IN.texCoord);
   //fragColour = vec4(vec3(gl_FragCoord.z), 1.0f);
}
