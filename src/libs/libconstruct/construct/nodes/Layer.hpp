#ifndef LAYER_H
#define LAYER_H

#include "construct/MeshData.hpp"
#include "construct/bv/BoundingVolume.hpp"

#include <QEnableSharedFromThis>
#include <QMatrix4x4>
#include <QSharedPointer>
#include <QVector>
#include <rhi/qrhi.h>


class RhiContext;
class Scene;

enum NodeCapability : quint32 {
	CAN_BE_SELECTED          = 1 << 0,
	CAN_BE_TRANSFORMED       = 1 << 1,
	HAS_CHILDREN             = 1 << 2,
	HAS_BOUNDING_VOLUME      = 1 << 3,
	HAS_GEOMETRY             = 1 << 4,
	REQUIRES_POST_PROCESSING = 1 << 5,
	CAN_BE_HIDDEN            = 1 << 6,
	SUPPORTS_CUSTOM_SHADING  = 1 << 7,
	DYNAMIC                  = 1 << 8,
	VOXEL_CHUNK              = 1 << 9
};

class Layer : public QEnableSharedFromThis<Layer>
{
protected:
	// The scene that this node belongs to.
	QSharedPointer<Scene> mScene;
	// The model matrix used for local transformations.
	QMatrix4x4 m_modelMatrix;
	
	// Child nodes.
	QVector<QSharedPointer<Layer>> mChildren;
	// Parent node (if any).
	QWeakPointer<Layer> mParent;
	// Bitmask of capabilities.
	quint32 mCapabilities;
	
public:
	Layer(QSharedPointer<Scene> scene)
		: mScene(scene)
		, mCapabilities{0}
	{}
	
public:
	virtual ~Layer() = default;
	
	// Lifecycle methods.
	virtual void initialize(RhiContext &context) = 0;
	virtual void update(RhiContext &context, qint64 interval) = 0;
	virtual void render(RhiContext &context) = 0;
	virtual void releaseResources() = 0;
	virtual QString toString() const = 0;
	
	// --- Scene Graph Transformations ---
	virtual void setTransform(const QMatrix4x4 &matrix) { m_modelMatrix = matrix; }
	// Compute full world transform (recursively include parent transforms).
	virtual QMatrix4x4 worldTransform() const {
		QMatrix4x4 parentTransform;
		if (auto p = mParent.lock())
			parentTransform = p->worldTransform();
		else
			parentTransform.setToIdentity();
		return parentTransform * m_modelMatrix;
	}
	
	// --- Scene Graph Hierarchy ---
	virtual void addChild(QSharedPointer<Layer> child) {
		mChildren.append(child);
		child->mParent = this->sharedFromThis();
	}
	virtual void removeChild(QSharedPointer<Layer> child) { Q_UNUSED(child); }
	virtual QVector<QSharedPointer<Layer>> children() const { return mChildren; }
	virtual QSharedPointer<Layer> parent() const { return mParent.lock(); }
	
	// --- Bounding Volume (For Spatial Queries) ---
	virtual BoundingVolume boundingVolume() const { return BoundingVolume(); }
	
	// --- Selection & Interaction ---
	virtual bool isSelectable() const { return mCapabilities & CAN_BE_SELECTED; }
	virtual bool isUnderCursor(const QVector3D &rayOrigin, const QVector3D &rayDir, float &outDistance) const { Q_UNUSED(rayOrigin); Q_UNUSED(rayDir); Q_UNUSED(outDistance); return false; }
	virtual bool isInsideLasso(const QVector<QVector2D> &lassoPoints, const QMatrix4x4 &viewProj) const { Q_UNUSED(lassoPoints); Q_UNUSED(viewProj); return false; }
	
	// --- Capabilities & Metadata ---
	virtual quint32 capabilities() const { return mCapabilities; }
	
	// --- Overlay/Gizmo Support ---
	// Returns a mesh used by the gizmo overlay system (e.g., a bounding box or simplified geometry).
	// Default implementation returns an empty mesh.
	virtual const MeshData &overlayMesh() const {
		static MeshData defaultMesh = { nullptr, nullptr, 0 };
		return defaultMesh;
	}
};

QDebug operator<<(QDebug dbg, const Layer &node);

#endif // LAYER_H
