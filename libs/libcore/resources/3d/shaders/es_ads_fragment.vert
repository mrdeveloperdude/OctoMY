#version 120

// Per vertex attributes
attribute highp vec3 vertexPosition;
attribute highp vec3 vertexNormal;

// Model View matrix, Normal matrix, and Model View Projection matrix
uniform highp mat4 MV;
uniform highp mat3 N;
uniform highp mat4 MVP;

// Output variables sent to fragment shader
// Interpolated by OpenGL before sent to fragment shader
varying highp vec3 interpolatedNormal;
varying highp vec3 interpolatedPosition;

void main(void)
{
    // Transform to eye coordinates
    interpolatedNormal = normalize( N * vertexNormal );
    interpolatedPosition = highp vec3( MV * highp vec4( vertexPosition, 1.0 ) );

    gl_Position = MVP * highp vec4( vertexPosition, 1.0 );
}
