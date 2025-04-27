#ifndef GIZMO_HPP
#define GIZMO_HPP

#include "OverlayStates.hpp"
#include "construct/MeshData.hpp"

#include <QMatrix4x4>
#include <QVector>

class Layer;
class RhiContext;
class OverlayStyleSheet;
class QRhiBuffer;


namespace Gizmo {

// Render per-node overlay (tint and outlines) based on active overlay states.
void renderNodeOverlay(
	  RhiContext &context
	, const Layer *node
	, OverlayStates states
	, const OverlayStyleSheet &styleSheet
	);
/*
// Render a composite multi-selection overlay (for a group of nodes).
void renderMultiSelectionOverlay(
	  RhiContext &context
	, const QVector<const SceneNode*> &selectedNodes
	, const OverlayStyleSheet &styleSheet
	);

// Render interactive handles for editing (move, rotate, scale, align).
void renderHandles(
	  RhiContext &context
	, const SceneNode *node
	, OverlayStates states
	, const OverlayStyleSheet &styleSheet
	);
*/
} // namespace Gizmo

#endif // GIZMO_HPP
