#version 120

// Light information
uniform highp vec4 lightPosition;
uniform highp vec3 lightIntensity;

// Material information
uniform lowp vec3 Ka;
uniform lowp vec3 Kd;
uniform lowp vec3 Ks;
uniform lowp float shininess;

// Input variables coming from vertex shader, interpolated to this fragment
varying highp vec3 interpolatedPosition;
varying highp vec3 interpolatedNormal;

void main()
{
    // normal has been interpolated, so we need to normalize it
    highp vec3 normalVector = normalize(interpolatedNormal);

    // Calculate light source vector
    highp vec3 lightSourceVector = normalize( lightPosition.xyz - interpolatedPosition);

    // Calculate the view vector
    highp vec3 viewVector = normalize( -interpolatedPosition.xyz );

    // Ambient contribution
    highp vec3 ambientContribution = Ka;

    // Default black diffuse and specular contributions
    highp vec3 diffuseContribution = highp vec3(0.0);
    highp vec3 specularContribution = highp vec3(0.0);

    // Dot product of two normalized vectors gives us a value
    // indicating how close they are to each other,
    // A value of -1 means they are pointing in opposite directions
    // while a value of 1 means they are pointing in the same direction.
    // A value of 0 means they are perpendicular to each other.
    // So if value is negative, there won't be any diffuse or specular contributions
    // since the light source cannot hit those areas
    if( dot( lightSourceVector, normalVector ) > 0.0 ) {

        // Diffuse Contribution
        diffuseContribution = Kd * dot( lightSourceVector, normalVector);

        // halfway vector, reduces the need for calculating a reflected vector,
        // which improvies performance slightly
        highp vec3 halfwayVector = normalize( viewVector + lightSourceVector);

        // Specular contribution
        specularContribution = Ks * pow( dot(halfwayVector, normalVector), shininess);
    }

    // Calculate final color
    gl_FragColor = highp vec4(lightIntensity * (ambientContribution + diffuseContribution + specularContribution), 1.0);
}
