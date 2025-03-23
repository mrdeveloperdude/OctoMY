#version 450

layout(set = 0, binding = 1) uniform LightInfo {
	vec4 lightPosition;
	vec3 lightIntensity;
};

layout(set = 0, binding = 2) uniform MaterialInfo {
	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
	float shininess;
};

// Add explicit locations to inputs
layout(location = 0) in vec3 interpolatedPosition;
layout(location = 1) in vec3 interpolatedNormal;

layout(location = 0) out vec4 fragColor;

void main()
{
	// Normalize the interpolated normal vector
	vec3 normalVector = normalize(interpolatedNormal);

	// Calculate light source vector
	vec3 lightSourceVector = normalize(lightPosition.xyz - interpolatedPosition);

	// Calculate view vector (from the fragment to the eye)
	vec3 viewVector = normalize(-interpolatedPosition);

	// Ambient contribution
	vec3 ambientContribution = Ka;

	// Default black diffuse and specular contributions
	vec3 diffuseContribution = vec3(0.0);
	vec3 specularContribution = vec3(0.0);

	// Only contribute if the light is facing the surface
	if (dot(lightSourceVector, normalVector) > 0.0) {
		// Diffuse contribution
		diffuseContribution = Kd * dot(lightSourceVector, normalVector);
		// Halfway vector for specular contribution
		vec3 halfwayVector = normalize(viewVector + lightSourceVector);
		specularContribution = Ks * pow(dot(halfwayVector, normalVector), shininess);
	}

	fragColor = vec4(lightIntensity * (ambientContribution + diffuseContribution + specularContribution), 1.0);
}
