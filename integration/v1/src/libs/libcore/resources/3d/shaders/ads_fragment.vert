#version 330 core

// Per vertex attributes
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexNormal;

// Model View matrix, Normal matrix, and Model View Projection matrix
uniform mat4 MV;
uniform mat3 N;
uniform mat4 MVP;

// Output variables sent to fragment shader
// Interpolated by OpenGL before sent to fragment shader
out vec3 interpolatedNormal;
out vec3 interpolatedPosition;

void main()
{
    // Transform to eye coordinates
    // The fragment shader values will be the interpolated value
    interpolatedNormal = normalize( N * vertexNormal );
    interpolatedPosition = vec3( MV * vec4( vertexPosition, 1.0 ) );

    gl_Position = MVP * vec4( vertexPosition, 1.0 );
}
