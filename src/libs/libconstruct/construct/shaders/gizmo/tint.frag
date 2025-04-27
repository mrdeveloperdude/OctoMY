#version 450
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform GizmoUniformBlock {
	mat4 mvp;
	vec4 tintColor;
	float tintOpacity;
	vec3 pad;
};

void main() {
	// Output the tint color with the specified opacity.
	fragColor = vec4(tintColor.rgb, tintOpacity);
}
