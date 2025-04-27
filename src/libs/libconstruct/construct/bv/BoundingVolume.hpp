#ifndef BOUNDINGVOLUME_HPP
#define BOUNDINGVOLUME_HPP

#include <QVector3D>

class BoundingVolume
{
private:
	QVector3D mMin;
	QVector3D mMax;
	
public:
	BoundingVolume() : mMin(0, 0, 0), mMax(0, 0, 0) {}
	BoundingVolume(const QVector3D &min, const QVector3D &max) : mMin(min), mMax(max) {}
	
	// Returns whether the bounding volume is valid
	bool isValid() const { return mMin != mMax; }
	
	// Expands the bounding volume to include a point
	void expand(const QVector3D &point) {
		mMin.setX(qMin(mMin.x(), point.x()));
		mMin.setY(qMin(mMin.y(), point.y()));
		mMin.setZ(qMin(mMin.z(), point.z()));
		mMax.setX(qMax(mMax.x(), point.x()));
		mMax.setY(qMax(mMax.y(), point.y()));
		mMax.setZ(qMax(mMax.z(), point.z()));
	}
	
	// Returns whether a ray intersects the AABB (for selection queries)
	bool intersectsRay(const QVector3D &rayOrigin, const QVector3D &rayDir, float &outDistance) const;
	
	// Getters
	QVector3D min() const { return mMin; }
	QVector3D max() const { return mMax; }
};

#endif // BOUNDINGVOLUME_HPP
