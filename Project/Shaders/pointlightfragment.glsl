#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform vec2 pixelSize;
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;
uniform mat4 inverseProjView;

out vec4 diffuseOutput;
out vec4 specularOutput;

float attenuate(vec3 lightDirection, float radius) {
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

void main(void){
   vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
   float depth = texture(depthTex, texCoord.xy).r;
   vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
   vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
   vec3 worldPos = invClipPos.xyz / invClipPos.w;

   //float dist = length(lightPos - worldPos);
   //float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

   vec3 dir = lightPos - worldPos;
   float atten = attenuate(dir, lightRadius);

   /*
   if(atten == 0.0){
      discard;
   }
   */

   vec3 normal = normalize(texture(normTex, texCoord.xy).xyz * 2.0 - 1.0);
   vec3 incident = normalize(lightPos - worldPos);
   vec3 viewDir = normalize(cameraPos - worldPos);
   vec3 halfDir = normalize(incident + viewDir);

   float lambert = clamp(dot(incident, normal), 0.0, 1.0);
   float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
   specFactor = pow(specFactor, 32.0);
   vec3 attenuated = lightColour.xyz * atten;
   diffuseOutput = vec4(attenuated * lambert, 1.0);
   //specularOutput = vec4(attenuated * specFactor, 1.0);
   specularOutput = vec4(vec3(specFactor) * atten, 1.0);

}
