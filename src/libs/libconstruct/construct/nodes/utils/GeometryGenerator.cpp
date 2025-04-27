#include "GeometryGenerator.hpp"
#include <cmath>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<float> GeometryGenerator::makeSphere(int subdivisions, float radius)
{
	// Step 1: Define an icosahedron.
	float t = (1.0f + std::sqrt(5.0f)) / 2.0f;
	std::vector<QVector3D> positions = {
		QVector3D(-1,  t,  0),
		QVector3D( 1,  t,  0),
		QVector3D(-1, -t,  0),
		QVector3D( 1, -t,  0),
		QVector3D( 0, -1,  t),
		QVector3D( 0,  1,  t),
		QVector3D( 0, -1, -t),
		QVector3D( 0,  1, -t),
		QVector3D( t,  0, -1),
		QVector3D( t,  0,  1),
		QVector3D(-t,  0, -1),
		QVector3D(-t,  0,  1)
	};
	for (auto &v : positions)
		v.normalize();
	
	// Step 2: Define initial indices for the icosahedron (20 triangles).
	unsigned int icosahedronIndices[] = {
		0, 11, 5,
		0, 5, 1,
		0, 1, 7,
		0, 7, 10,
		0, 10, 11,
		1, 5, 9,
		5, 11, 4,
		11, 10, 2,
		10, 7, 6,
		7, 1, 8,
		3, 9, 4,
		3, 4, 2,
		3, 2, 6,
		3, 6, 8,
		3, 8, 9,
		4, 9, 5,
		2, 4, 11,
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
	};
	std::vector<unsigned int> indices(std::begin(icosahedronIndices), std::end(icosahedronIndices));
	
	// Step 3: Subdivide each triangle 'subdivisions' times.
	for (int i = 0; i < subdivisions; ++i) {
		std::vector<unsigned int> newIndices;
		std::map<std::pair<unsigned int, unsigned int>, unsigned int> midpointCache;
		auto getMidpoint = [&](unsigned int i1, unsigned int i2) -> unsigned int {
			std::pair<unsigned int, unsigned int> key = { std::min(i1, i2), std::max(i1, i2) };
			if (midpointCache.count(key))
				return midpointCache[key];
			QVector3D mid = (positions[i1] + positions[i2]) * 0.5f;
			mid.normalize();
			positions.push_back(mid);
			unsigned int index = positions.size() - 1;
			midpointCache[key] = index;
			return index;
		};
		for (size_t j = 0; j < indices.size(); j += 3) {
			unsigned int i0 = indices[j], i1 = indices[j+1], i2 = indices[j+2];
			unsigned int a = getMidpoint(i0, i1);
			unsigned int b = getMidpoint(i1, i2);
			unsigned int c = getMidpoint(i2, i0);
			newIndices.push_back(i0); newIndices.push_back(a); newIndices.push_back(c);
			newIndices.push_back(i1); newIndices.push_back(b); newIndices.push_back(a);
			newIndices.push_back(i2); newIndices.push_back(c); newIndices.push_back(b);
			newIndices.push_back(a);  newIndices.push_back(b); newIndices.push_back(c);
		}
		indices = newIndices;
	}
	
	// Step 4: Project vertices onto sphere.
	for (auto &v : positions)
		v = v.normalized() * radius;
	
	// Step 5: Compute normals (same as positions normalized) and spherical UVs.
	std::vector<QVector3D> normals(positions.size());
	std::vector<QVector2D> uvs(positions.size());
	for (size_t i = 0; i < positions.size(); ++i) {
		normals[i] = positions[i].normalized();
		float u = 0.5f + atan2(positions[i].z(), positions[i].x()) / (2.0f * M_PI);
		float v = 0.5f - asin(positions[i].y() / radius) / M_PI;
		uvs[i] = QVector2D(u, v);
	}
	
	// Step 6: Build interleaved vertex data.
	// Format: position (3), uv (2), normal (3) per vertex.
	std::vector<float> sphereData;
	for (unsigned int idx : indices) {
		QVector3D pos = positions[idx];
		QVector2D uv = uvs[idx];
		QVector3D norm = normals[idx];
		sphereData.push_back(pos.x());
		sphereData.push_back(pos.y());
		sphereData.push_back(pos.z());
		sphereData.push_back(uv.x());
		sphereData.push_back(uv.y());
		sphereData.push_back(norm.x());
		sphereData.push_back(norm.y());
		sphereData.push_back(norm.z());
	}
	return sphereData;
}
