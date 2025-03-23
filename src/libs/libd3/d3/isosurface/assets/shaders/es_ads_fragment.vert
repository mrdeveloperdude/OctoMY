#version 450

// Input vertex attributes with explicit location qualifiers
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

// Uniform block for matrices with descriptor set and binding
layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 MV;
	mat3 N;
	mat4 MVP;
};

// Output variables with explicit location qualifiers
layout(location = 0) out vec3 interpolatedNormal;
layout(location = 1) out vec3 interpolatedPosition;

void main(void)
{
	// Transform to eye coordinates
	interpolatedNormal = normalize(N * vertexNormal);
	interpolatedPosition = vec3(MV * vec4(vertexPosition, 1.0));

	gl_Position = MVP * vec4(vertexPosition, 1.0);
}
