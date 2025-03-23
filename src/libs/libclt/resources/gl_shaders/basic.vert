#version 450

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 texCoord;
layout(location = 0) out vec4 texc;

layout(set = 0, binding = 0) uniform MatrixBlock {
	mat4 matrix;
};

void main(void)
{
	gl_Position = matrix * vertex;
	texc = texCoord;
}
