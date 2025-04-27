#include "InputHandler.hpp"
#include <QDebug>
#include <QWidget>
#include <QWindow>

InputHandler::InputHandler(QObject *parent)
	: QObject(parent)
{
	inputMapping.defaultMapping();
}


const auto CLASSIC_GRAB=false;
void InputHandler::grabMouse(bool grab){
	if (auto widget = qobject_cast<QWidget*>(parent())) {
		if(CLASSIC_GRAB){
			if(grab){
				widget->grabMouse();
			}
			else{
				widget->releaseMouse();
			}
		}
		else{
			widget->windowHandle()->setMouseGrabEnabled(grab);
		}
		m_mouseCaptured = grab;
	}
}

void InputHandler::processInput()
{
	// Evaluate the mapping entry.
	const auto entry = inputMapping.evaluate(currentState);
	//grabMouse(entry.captureMouse);

	// Process actions (e.g., dolly, pan, rotate, etc.).
	switch (entry.action) {
	case InputAction::Dolly:
		emit dollyTriggered(inputMapping.dollySensitivity * entry.multiplierWAxis * currentState.scrollDelta);
		break;
	case InputAction::Pan: {
		QPointF delta;
		if (entry.fpsDelta) {
			if (!currentState.panActive) {
				currentState.panActive = true;
				currentState.lastPanPos = currentState.mousePos;
				delta = QPointF(0, 0);
			} else {
				delta = currentState.mousePos - currentState.lastPanPos;
				currentState.lastPanPos = currentState.mousePos;
			}
		} else {
			delta = currentState.mousePos - currentState.rightDownPos;
		}
		emit panTriggered(inputMapping.panSensitivity * QPointF(delta.x() * entry.multiplierXAxis, delta.y() * entry.multiplierYAxis));
		break;
	}
	case InputAction::Look: {
		if (!currentState.lookActive) {
			currentState.lookActive = true;
			currentState.lastLookPos = currentState.mousePos;
		} else {
			QPointF delta = currentState.mousePos - currentState.lastLookPos;
			currentState.lastLookPos = currentState.mousePos;
			emit rotateTriggered(inputMapping.rotateSensitivity * QPointF(delta.x() * entry.multiplierXAxis, delta.y() * entry.multiplierYAxis));
		}
		break;
	}
	case InputAction::Orbit: {
		if (!currentState.orbitActive) {
			currentState.orbitActive = true;
			currentState.lastOrbitPos = currentState.mousePos;
		} else {
			QPointF delta = currentState.mousePos - currentState.lastOrbitPos;
			currentState.lastOrbitPos = currentState.mousePos;
			emit orbitTriggered(inputMapping.orbitSensitivity * QPointF(delta.x() * entry.multiplierXAxis, delta.y() * entry.multiplierYAxis));
		}
		break;
	}
	
	
	case InputAction::Select:
		emit selectTriggered(currentState.mousePos);
		break;
	case InputAction::MultiSelect:
		emit multiSelectTriggered(QRectF(currentState.leftDownPos, currentState.mousePos));
		break;
	case InputAction::ToggleSelection:
		emit toggleSelectionTriggered(currentState.mousePos);
		break;
	case InputAction::ResetView:
		emit resetViewTriggered();
		break;
	default:
		// Reset active flags for actions that require incremental deltas.
		currentState.lookActive = false;
		currentState.panActive = false;
		currentState.orbitActive = false;
		break;
	}
	
	// Reset scroll delta after processing.
	currentState.scrollDelta = 0.0;
}




void InputHandler::handleModifiers(QInputEvent *event){
	// Update modifier state.
	currentState.shiftDown = event->modifiers() & Qt::ShiftModifier;
	currentState.ctrlDown  = event->modifiers() & Qt::ControlModifier;
	currentState.altDown   = event->modifiers() & Qt::AltModifier;
	currentState.metaDown  = event->modifiers() & Qt::MetaModifier;
}

void InputHandler::handleMousePress(QMouseEvent *event)
{
	//qDebug() << "mp";
	currentState.mousePos = event->position();
	switch (event->button()) {
	case Qt::LeftButton:
		currentState.leftButtonDown = true;
		currentState.leftDownPos = event->position();
		break;
	case Qt::RightButton:
		currentState.rightButtonDown = true;
		currentState.rightDownPos = event->position();
		break;
	case Qt::MiddleButton:
		currentState.middleButtonDown = true;
		currentState.middleDownPos = event->position();
		break;
	default:
		break;
	}
	handleModifiers(event);
}


void InputHandler::handleMouseRelease(QMouseEvent *event)
{
	//qDebug()<< "mr";
	currentState.mousePos = event->position();
	switch (event->button()) {
	case Qt::LeftButton:
		currentState.leftButtonDown = false;
		break;
	case Qt::RightButton:
		currentState.rightButtonDown = false;
		break;
	case Qt::MiddleButton:
		currentState.middleButtonDown = false;
		break;
	default:
		break;
	}
	handleModifiers(event);
}

void InputHandler::handleMouseMove(QMouseEvent *event)
{
	//qDebug()<< "mm";
	currentState.mousePos = event->position();
	handleModifiers(event);
}

void InputHandler::handleMouseWheelEvent(QWheelEvent *event)
{
	//qDebug()<< "mw";
	currentState.scrollDelta += event->angleDelta().y();
}

void InputHandler::handleKeyPress(QKeyEvent *event)
{
	//qDebug()<< "kp";
	currentState.pressedKeys.insert(event->key());
	handleModifiers(event);
}

void InputHandler::handleKeyRelease(QKeyEvent *event)
{
	//qDebug()<< "kr";
	currentState.pressedKeys.remove(event->key());
	handleModifiers(event);
}
