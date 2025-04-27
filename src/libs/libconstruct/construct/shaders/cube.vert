#version 450
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoord;
layout(location = 2) out vec3 Normal;

layout(std140, binding = 0) uniform VertexUBO {
	mat4 mvp;    // projection * view * model
	mat4 model;  // model matrix
};

void main() {
	vec4 worldPos = model * vec4(aPos, 1.0);
	FragPos = worldPos.xyz;
	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	TexCoord = aTexCoord;
	gl_Position = mvp * vec4(aPos, 1.0);
}
