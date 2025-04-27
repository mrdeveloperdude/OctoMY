#version 450
layout(location = 0) in vec3 aPos;

layout(std140, binding = 0) uniform GizmoUniformBlock {
	mat4 mvp;
	vec4 tintColor;
	float tintOpacity;
	vec3 pad;
};

void main() {
	gl_Position = mvp * vec4(aPos, 1.0);
}
