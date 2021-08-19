#version 330 core

uniform float near;
uniform float far;
uniform sampler2D texture_diffuse1;

in vec2 TexCoord;
out vec4 FragColor;


float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	FragColor = vec4(vec3(depth), 1.0f);
	//FragColor = texture(texture_diffuse1,TexCoord);
}
