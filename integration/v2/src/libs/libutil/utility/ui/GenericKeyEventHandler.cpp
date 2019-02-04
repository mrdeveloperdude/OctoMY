#include "GenericKeyEventHandler.hpp"

#include <QDebug>
#include <QKeyEvent>

GenericKeyEventHandler::GenericKeyEventHandler(QObject* parent)
	: QObject(parent)
{
	if(nullptr!= parent){
		parent->installEventFilter(this);
	}
}
GenericKeyEventHandler::~GenericKeyEventHandler()
{

}

bool GenericKeyEventHandler::eventFilter(QObject *obj, QEvent *event)
{

	if(nullptr != eventProcessor && parent() == obj && nullptr != event  ) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
		if(nullptr!=keyEvent && eventProcessor(obj, keyEvent)){
			event->accept();
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}
void GenericKeyEventHandler::setEventProcessor(std::function<bool(QObject *, QKeyEvent *)> processor)
{
	eventProcessor = processor;
}

