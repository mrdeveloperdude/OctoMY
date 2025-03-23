#version 450

struct Vertex {
	vec3 pos;
	float scale;
	vec3 normal;
	float size;
};

layout(std430, binding = 1) buffer GeomBuffer {
	Vertex vertices[];
};

layout(set = 0, binding = 0) uniform Matrices {
	mat4 p3d_ModelViewMatrix;
	mat4 p3d_ProjectionMatrix;
};

layout(location = 0) out vec2 coord;
layout(location = 1) out vec3 color;
layout(location = 2) out vec3 normal_worldspace;

void main() {
	vec4 pos = p3d_ModelViewMatrix * vec4(vertices[gl_VertexIndex].pos, 1.0);
	coord = vec2(0.0); // Default value if unused
	color = vertices[gl_VertexIndex].normal;
	normal_worldspace = vertices[gl_VertexIndex].normal;
	gl_Position = p3d_ProjectionMatrix * pos;
}
