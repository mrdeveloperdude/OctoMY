#ifndef INPUTSTATE_HPP
#define INPUTSTATE_HPP

#include <QPointF>
#include <QSet>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

enum class MouseButton {
	Left,
	Right,
	Middle,
	Unknown
};


struct InputState {
	// Mouse button states.
	bool leftButtonDown   = false;
	bool rightButtonDown  = false;
	bool middleButtonDown = false;
	
	// The positions where each button was pressed.
	QPointF leftDownPos;
	QPointF rightDownPos;
	QPointF middleDownPos;
	
	// Current mouse position.
	QPointF mousePos;
	QPointF lastLookPos;
	bool lookActive = false;
	QPointF lastPanPos;
	bool panActive = false;
	QPointF lastOrbitPos;
	bool orbitActive = false;
	
	// Accumulated scroll delta for this frame.
	qreal scrollDelta = 0.0;
	
	// Set of currently pressed key codes.
	QSet<int> pressedKeys;
	
	// Modifier keys.
	bool shiftDown = false;
	bool ctrlDown  = false;
	bool altDown   = false;
	bool metaDown  = false;
};


#endif // INPUTSTATE_HPP
