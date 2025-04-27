#version 450

layout(location = 0) in vec2 aPosition;

void main()
{
	// The full-screen quad is already in NDC.
	gl_Position = vec4(aPosition, 0.0, 1.0);
}
