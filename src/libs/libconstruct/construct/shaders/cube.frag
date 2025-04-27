#version 450
layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec2 TexCoord;
layout(location = 2) in vec3 Normal;

layout(location = 0) out vec4 fragColor;

layout(std140, binding = 1) uniform FragmentUBO {
	mat4 mvp;         // Not used here
	vec3 lightDir;
	float padding1;
	vec3 cameraPos;
	float padding2;
	vec3 objectColor; 
	float padding3;
};

void main() {
	vec3 norm = normalize(Normal);
	vec3 light = normalize(-lightDir);
	vec3 viewDir = normalize(cameraPos - FragPos);
	
	float diff = max(dot(norm, light), 0.0);
	vec3 diffuse = diff * vec3(1.0);
	
	vec3 halfwayDir = normalize(light + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
	vec3 specular = spec * vec3(1.0) * 0.5;
	
	float edge = pow(1.0 - max(dot(norm, viewDir), 0.0), 3.0);
	vec3 edgeColor = vec3(0.2) * edge;
	
	vec3 result = (diffuse + specular + edgeColor) * objectColor;
	fragColor = vec4(result, 1.0);
}
