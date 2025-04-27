#version 440
layout(location = 0) in vec2 v_uv;  // Fullscreen quad texture coordinates
layout(location = 0) out vec4 fragColor;

layout(std140, binding = 0) uniform buf {
	mat4 mvp;
	mat4 inv_view_matrix; // Inverse of the camera's view matrix
	mat4 inv_projection;  // Inverse projection matrix for ray reconstruction
	vec4 camera_position; // Camera position in world space
	vec4 center_offset;
	vec4 grid_color;
	vec4 checker_color_a;
	vec4 checker_color_b;
	float unit_size;
	float major_line_thickness;
	float minor_line_thickness;
	float minor_line_alpha;
	float fade_start;
	float fade_end;
	float subdivisions;
	float pad;
};

// Local bits per channel (for example, 8-bit per channel)
const int bitsPerChannel = 8;
const float MAGIC_DITHER_NUMBER = 8.0;

//-------------------------------------------------------------------
// Noise functions for dithering

float ditherNoise(vec2 pos) {
	return fract(sin(dot(pos, vec2(12.9898, 78.233))) * 43758.5453);
}

float generateDither(vec2 pos, int bits) {
	float quantStep = 1.0 / (pow(2.0, float(bits)) - 1.0);
	return (ditherNoise(pos) - 0.5) * quantStep * MAGIC_DITHER_NUMBER;
}

vec3 applyDither(vec3 color, int bits) {
	float noise = generateDither(gl_FragCoord.xy, bits);
	return color + noise;
}

//-------------------------------------------------------------------
// Helper functions

float grid(vec2 pos, float unit, float thickness) {
	unit = max(unit, 1e-6);
	vec2 threshold = max(fwidth(pos) * thickness * 0.5 / unit, 1e-6);
	vec2 posWrapped = pos / unit;
	vec2 line = step(fract(-posWrapped), threshold) + step(fract(posWrapped), threshold);
	return max(line.x, line.y);
}

float checker(vec2 pos, float unit) {
	float square1 = step(0.5, fract(pos.x / unit * 0.5));
	float square2 = step(0.5, fract(pos.y / unit * 0.5));
	return max(square1, square2) - square1 * square2;
}

vec4 getSkyColor(vec3 world_dir) {
	vec3 horizonColor = vec3(0.7, 0.8, 1.0);
	vec3 midColor     = vec3(0.4, 0.6, 1.0);
	vec3 zenithColor  = vec3(0.0, 0.1, 0.3);
	
	float t = clamp(world_dir.y, 0.0, 1.0);
	if (t < 0.5) {
		float localT = t / 0.5;
		return vec4(mix(horizonColor, midColor, localT), 1.0);
	} else {
		float localT = (t - 0.5) / 0.5;
		return vec4(mix(midColor, zenithColor, localT), 1.0);
	}
}

vec4 getGroundGridColor(vec3 world_pos) {
	float safeSubdivisions = max(1.0, subdivisions);
	float majorGrid = grid(world_pos.xz, unit_size, major_line_thickness);
	float minorGrid = grid(world_pos.xz, unit_size / safeSubdivisions, minor_line_thickness) * minor_line_alpha;
	float gridFactor = clamp(majorGrid + minorGrid, 0.0, 1.0);
	
	float pattern = checker(world_pos.xz, unit_size);
	vec4 checkerColor = mix(checker_color_a, checker_color_b, pattern);
	
	vec3 composite = mix(checkerColor.rgb, grid_color.rgb, gridFactor);
	return vec4(composite, 1.0);
}

//-------------------------------------------------------------------
// Main shader entry point

void main() {
	// Reconstruct view-space ray direction.
	vec4 clipPos = vec4(v_uv * 2.0 - 1.0, 1.0, 1.0);
	vec4 viewPos = inv_projection * clipPos;
	viewPos /= viewPos.w; // perspective divide to get view-space position
	vec3 camera_dir = normalize(viewPos.xyz);
	vec3 world_dir = normalize((inv_view_matrix * vec4(camera_dir, 0.0)).xyz);
	
	// Compute the sky color.
	vec4 skyColor = getSkyColor(world_dir);
	
	// Calculate intersection with the ground plane (y = 0).
	float t = -camera_position.y / world_dir.y;
	if (t < 0.0) {
		// No ground hit; output sky color with dithering.
		fragColor = vec4(applyDither(skyColor.rgb, bitsPerChannel), 1.0);
		gl_FragDepth = 1.0; // Use far plane depth for sky
		return;
	}
	
	// Ground hit: compute the world position.
	vec3 world_pos = camera_position.xyz + t * world_dir;
	
	// Calculate fade factor relative to the camera's xz position.
	vec2 camPosXZ = camera_position.xz;
	float fadeDistance = distance(world_pos.xz, camPosXZ);
	float fadeFactor = 1.0 - clamp((fadeDistance - fade_start) / (fade_end - fade_start), 0.0, 1.0);
	
	// Get the ground grid color.
	vec4 groundColor = getGroundGridColor(world_pos);
	
	// Composite the ground over the sky.
	vec3 finalColor = mix(skyColor.rgb, groundColor.rgb, fadeFactor);
	
	// Apply noise dithering to the final color.
	finalColor = applyDither(finalColor, bitsPerChannel);
	fragColor = vec4(finalColor, 1.0);
	
	// Compute depth: transform the ground hit position to clip space.
	vec4 clipPosFromWorld = mvp * vec4(world_pos, 1.0);
	clipPosFromWorld /= clipPosFromWorld.w;
	// Convert clip-space z (range [-1, 1]) to depth range [0, 1].
	gl_FragDepth = clipPosFromWorld.z * 0.5 + 0.5;
}


