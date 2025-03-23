#version 450

layout(location = 0) in vec4 texc;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D texSampler;

void main(void)
{
	fragColor = texture(texSampler, texc.st);
}
