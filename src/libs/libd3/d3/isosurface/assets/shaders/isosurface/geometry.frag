#version 450

layout(location = 0) in vec2 coord;
layout(location = 1) in vec3 color;

layout(location = 0) out vec4 p3d_FragColor;

void main() {
	p3d_FragColor = vec4(color, 1.0);
}
