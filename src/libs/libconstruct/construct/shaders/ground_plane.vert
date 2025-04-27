#version 440
layout(location = 0) in vec3 position;  // NDC coordinates
layout(location = 1) in vec2 texcoord;  // Unused

layout(location = 0) out vec2 v_uv;  // Pass texture coordinates

void main()
{
	v_uv = texcoord;
	gl_Position = vec4(position, 1.0);  // Direct pass-through
}
