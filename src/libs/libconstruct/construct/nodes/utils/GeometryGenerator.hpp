#ifndef GEOMETRYGENERATOR_HPP
#define GEOMETRYGENERATOR_HPP

#include <vector>
#include <QVector3D>
#include <QVector2D>

class GeometryGenerator {
public:
	// Generates interleaved vertex data for a sphere.
	// Format per vertex: position (3 floats), UV (2 floats), normal (3 floats).
	// 'subdivisions' specifies how many times to subdivide the initial icosahedron.
	// 'radius' is the sphere radius.
	static std::vector<float> makeSphere(int subdivisions, float radius);
};

#endif // GEOMETRYGENERATOR_HPP
