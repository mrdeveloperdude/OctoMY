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

layout(location = 0) in vec3 interpolatedPosition;
layout(location = 1) in vec3 interpolatedNormal;

layout(location = 0) out vec4 fragmentColor;

void main()
{
	// Normalize the interpolated normal vector
	vec3 normalVector = normalize(interpolatedNormal);

	// Calculate light source vector
	vec3 lightSourceVector = normalize(lightPosition.xyz - interpolatedPosition);

	// Calculate view vector (from fragment to eye)
	vec3 viewVector = normalize(-interpolatedPosition);

	// Ambient contribution
	vec3 ambientContribution = Ka;

	// Default black diffuse and specular contributions
	vec3 diffuseContribution = vec3(0.0);
	vec3 specularContribution = vec3(0.0);

	if (dot(lightSourceVector, normalVector) > 0.0) {
		// Diffuse contribution
		diffuseContribution = Kd * dot(lightSourceVector, normalVector);

		// Halfway vector for specular contribution
		vec3 halfwayVector = normalize(viewVector + lightSourceVector);
		specularContribution = Ks * pow(dot(halfwayVector, normalVector), shininess);
	}

	// Calculate final color
	fragmentColor = vec4(lightIntensity * (ambientContribution + diffuseContribution + specularContribution), 1.0);
}
