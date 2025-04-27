#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

#include <QObject>
#include "InputState.hpp"
#include "InputMapping.hpp"

class InputHandler : public QObject {
	Q_OBJECT
public:
	explicit InputHandler(QObject *parent = nullptr);
	
	// Current raw input state.
	InputState currentState;
	// The mapping from raw inputs to actions.
	InputMapping inputMapping;
	// For FPS-style rotation, store the previous mouse position for incremental deltas.
	QPointF m_previousRotationPos;
	
	bool m_mouseCaptured = false;
	
	// Process the current state and emit corresponding signals.
	void processInput();
	
	void grabMouse(bool grab);
	
	// Update the input state from widget event handlers.
	void handleModifiers(QInputEvent *event);
	void handleMousePress(QMouseEvent *event);
	void handleMouseRelease(QMouseEvent *event);
	void handleMouseMove(QMouseEvent *event);
	void handleMouseWheelEvent(QWheelEvent *event);
	void handleKeyPress(QKeyEvent *event);
	void handleKeyRelease(QKeyEvent *event);
	
private:
	void defaultMapping();

	
signals:
	// These signals can be connected to your camera or selection routines.
	void dollyTriggered(qreal amount);
	void panTriggered(const QPointF &delta);
	void rotateTriggered(const QPointF &delta);
	void orbitTriggered(const QPointF &delta);
	void selectTriggered(const QPointF &pos);
	void multiSelectTriggered(const QRectF &rect);
	void toggleSelectionTriggered(const QPointF &pos);
	void resetViewTriggered();
};

#endif // INPUTHANDLER_HPP
