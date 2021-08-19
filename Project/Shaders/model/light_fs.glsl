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
layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight light[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer LightIndicesBuffer {
	LightIndex index[];
} lightIndicesBuffer;

// Uniforms
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform int numberOfTilesX;
uniform int lightCount;

out vec4 fragColour;

float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

void main() {
	ivec2 position = ivec2(gl_FragCoord.xy);
	ivec2 groupID = position / ivec2(16, 16);
	uint index = groupID.y * numberOfTilesX + groupID.x;

	vec4 base_diffuse = texture(texture_diffuse1, IN.texCoords);
	vec4 base_specular = texture(texture_specular1, IN.texCoords);
	vec3 normal = texture(texture_normal1, IN.texCoords).rgb;
	normal = normalize(normal * 2.0 - 1.0);
	vec4 colour = vec4(0.0, 0.0, 0.0, 1.0);

	vec3 viewDirection = normalize(IN.ViewPos - IN.FragPos);

	uint offset = index * lightCount;
	for (uint i = 0; i < lightCount && lightIndicesBuffer.index[offset + i].index != -1; i++) {
		uint lightIndex = lightIndicesBuffer.index[offset + i].index;
		PointLight light = lightBuffer.light[lightIndex];

		vec4 lightColour = light.colour;
		vec3 lightPos = IN.TBN * vec3(light.position.xyz);
		float lightRadius = light.paddingAndRadius.w;

		vec3 lightDirection = lightPos - IN.FragPos;
		float attenuation = attenuate(lightDirection, lightRadius);

		lightDirection = normalize(lightDirection);
		vec3 halfDir = normalize(lightDirection + viewDirection);

		float diffuse = max(dot(lightDirection, normal), 0.0);
		float specular = max(dot(normal, halfDir), 0.0);
		specular = pow(specular, 32.0);

		if (diffuse == 0.0) {
			specular = 0.0;
		}

		vec3 irradiance = lightColour.rgb * (base_diffuse.rgb * diffuse) *attenuation;
		irradiance += lightColour.rgb * (base_specular.rgb * vec3(specular)) * attenuation;
		colour.rgb += irradiance;
	}

	colour.rgb += base_diffuse.rgb * 0.08;

	if (base_diffuse.a <= 0.2) {
		discard;
	}

	fragColour = colour;
}
