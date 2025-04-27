#include "BoundingVolume.hpp"

bool BoundingVolume::intersectsRay(const QVector3D &rayOrigin, const QVector3D &rayDir, float &outDistance) const {
	if (!isValid()) return false;
	
	QVector3D invDir = QVector3D(1.0f / rayDir.x(), 1.0f / rayDir.y(), 1.0f / rayDir.z());
	QVector3D tMin = (mMin - rayOrigin) * invDir;
	QVector3D tMax = (mMax - rayOrigin) * invDir;
	
	QVector3D t1 = QVector3D(qMin(tMin.x(), tMax.x()), qMin(tMin.y(), tMax.y()), qMin(tMin.z(), tMax.z()));
	QVector3D t2 = QVector3D(qMax(tMin.x(), tMax.x()), qMax(tMin.y(), tMax.y()), qMax(tMin.z(), tMax.z()));
	
	float tNear = qMax(qMax(t1.x(), t1.y()), t1.z());
	float tFar = qMin(qMin(t2.x(), t2.y()), t2.z());
	
	if (tNear > tFar || tFar < 0.0f) return false;
	outDistance = tNear;
	return true;
}
