#version 450

// Uniform block matching our dynamic uniform buffer layout:
// [ iTime, iResolution.x, iResolution.y, iMouse.x, iMouse.y, iChannelResolution.x, iChannelResolution.y, padding ]
layout(std140, set = 0, binding = 0) uniform Params {
	float iTime;
	float iResolutionX;
	float iResolutionY;
	float iMouseX;
	float iMouseY;
	float iChannelResolutionX;
	float iChannelResolutionY;
	float pad0;
};

layout(set = 0, binding = 1) uniform sampler2D iChannel0;

layout(location = 0) out vec4 fragColor;


// --- Constants ---
const float MAX_STEPS = 1024.0;
const float SX = 76.0;
const float SY = 48.0;
const float SZ = 80.0;

const float dval = 0.05;
const float dd = 1.0;
const float _density = 1.0;

const float MinVal = 0.3;
const float MaxVal = 0.5;

vec3 Bounds = vec3(0.8 * SX, 0.7 * SY, abs(0.5) * SZ);

// --- Helper Functions ---

float hash1(float seed)
{
	return fract(sin(seed) * 43758.5453123);
}

bool box(in vec3 ro, in vec3 rd, in vec3 sz, out float tN, out float tF, out vec3 n)
{
	vec3 m = 1.0 / rd;
	vec3 k = abs(m) * sz;
	vec3 a = -m * ro - k * 0.5;
	vec3 b = a + k;
	n = -sign(rd) * step(a.yzx, a.xyz) * step(a.zxy, a.xyz);
	tN = max(max(a.x, a.y), a.z);
	tF = min(min(b.x, b.y), b.z);
	return tN < tF;
}

// --- Bicubic Sampling ---
#define BICUBIC
vec4 BS_A = vec4(3.0, -6.0, 0.0, 4.0) / 6.0;
vec4 BS_B = vec4(-1.0, 6.0, -12.0, 8.0) / 6.0;

vec4 powers(float x)
{
	return vec4(x * x * x, x * x, x, 1.0);
}

float spline(float x, float c0, float c1, float c2, float c3)
{
	return c0 * dot(BS_B, powers(x + 1.0)) +
	       c1 * dot(BS_A, powers(x)) +
	       c2 * dot(BS_A, powers(1.0 - x)) +
	       c3 * dot(BS_B, powers(2.0 - x));
}

#define SAM(a,b) textureLod(iChannel0, (i + vec2(a, b) + 0.5) / res, 0.0).g

float texture_Bicubic(vec2 t)
{
	vec2 res = vec2(iChannelResolutionX, iChannelResolutionY);
	vec2 p = res * t - 0.5;
	vec2 f = fract(p);
	vec2 i = floor(p);
	return spline(f.y,
		spline(f.x, SAM(-1, -1), SAM(0, -1), SAM(1, -1), SAM(2, -1)),
		spline(f.x, SAM(-1, 0), SAM(0, 0), SAM(1, 0), SAM(2, 0)),
		spline(f.x, SAM(-1, 1), SAM(0, 1), SAM(1, 1), SAM(2, 1)),
		spline(f.x, SAM(-1, 2), SAM(0, 2), SAM(1, 2), SAM(2, 2))
	);
}

#ifdef BICUBIC
float valueAt(in vec3 p)
{
	p *= 2.2;
	float cval0 = texture_Bicubic(0.6 * p.xy / MAX_STEPS);
	float cval1 = texture_Bicubic(0.5 * p.xz / MAX_STEPS);
	float v0 = sqrt(cval0 * cval1);
	return clamp(0.1 + 0.3 * abs(cos(iTime * 0.1)) + 0.5 * v0, 0.0, 1.0);
}
#else
float valueAt(in vec3 p)
{
	p *= 1.8;
	vec4 cval0 = textureLod(iChannel0, 0.6 * p.xy / MAX_STEPS, 0.0);
	vec4 cval1 = textureLod(iChannel0, 0.5 * p.xz / MAX_STEPS, 0.0);
	float v0 = sqrt(cval0.g * cval1.b) + (cval0.r * cval1.g);
	return clamp(0.1 + 0.3 * abs(cos(iTime * 0.1)) + 0.5 * v0, 0.0, 1.0);
}
#endif

vec3 normalAt(in vec3 p)
{
	vec2 e = vec2(0.1, -0.1);
	return normalize(
		e.xyy * valueAt(p + e.xyy) +
		e.yyx * valueAt(p + e.yyx) +
		e.yxy * valueAt(p + e.yxy) +
		e.xxx * valueAt(p + e.xxx)
	);
}

vec3 doShading(vec3 rd, vec3 pos, vec3 nor, vec3 lig, vec3 col, float t)
{
	vec3 ref = reflect(rd, nor);
	float amb = 0.9;
	float dif = clamp(dot(nor, lig), 0.0, 1.0);
	float fre = pow(clamp(1.0 + dot(nor, rd), 0.0, 1.0), 2.0);
	float spe = pow(clamp(dot(ref, lig), 0.0, 1.0), 16.0);
	vec3 brdf = vec3(0.0);
	brdf += 1.20 * dif * vec3(1.00, 0.90, 0.60);
	brdf += 1.20 * spe * vec3(1.00, 0.90, 0.60) * dif;
	brdf += 0.30 * amb * vec3(0.50, 0.70, 1.00);
	brdf += 0.40 * fre * vec3(1.00, 1.00, 1.00);
	brdf += 0.4;
	return col * brdf;
}

vec3 heatmapGradient(in float t)
{
	return clamp(
		(pow(t, 1.5) * 0.8 + 0.2) *
		vec3(smoothstep(0.0, 0.35, t) + t * 0.5, smoothstep(0.5, 1.0, t), max(1.0 - t * 1.7, t * 7.0 - 6.0)),
		0.0,
		1.0
	);
}

vec3 getColor(float val)
{
	return heatmapGradient(1.5 * val);
}

// --- Main Function ---
void main()
{
	// Obtain screen coordinate and resolution vector.
	vec2 fragCoord = gl_FragCoord.xy;
	vec2 res = vec2(iResolutionX, iResolutionY);
	vec2 q = fragCoord / res;
	
	// Light direction
	vec3 lightDir = normalize(vec3(1.0, -5.0, 6.5));
	
	// Define a simple camera ray.
	vec3 ro = vec3(180.0 * cos(0.5 * iTime), 180.0 * cos(0.5 * iTime * 0.3), 180.0 * sin(0.5 * iTime));
	vec3 rd = normalize(vec3(2.0 * q - 1.0, 5.0));

	// Background color based on screen position.
	vec3 col = vec3(0.22, 0.26, 0.28) * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.3);
	
	float tN, tF;
	vec3 n;
	if (box(ro, rd, Bounds, tN, tF, n) && tN > 0.0)
	{
		vec3 p = ro + rd * tN;
		float v0 = valueAt(p);
		if (v0 >= MinVal && v0 <= MaxVal)
		{
			col = doShading(rd, p, n, lightDir, getColor(v0), tN);
		}
		else
		{
			float val = (v0 <= MinVal) ? MinVal : MaxVal;
			// A simple fallback – advance the ray a bit further.
			p = ro + rd * (tN + 10.0);
			col = doShading(rd, p, n, lightDir, getColor(val), tN);
		}
	}
	fragColor = vec4(col, 1.0);
}
