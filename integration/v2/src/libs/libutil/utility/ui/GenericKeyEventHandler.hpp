#ifndef GENERICKEYEVENTHANDLER_HPP
#define GENERICKEYEVENTHANDLER_HPP

#include <QObject>
#include <QString>
#include <functional>

class QKeyEvent;
class QEvent;

class  GenericKeyEventHandler: public QObject
{
		Q_OBJECT
	private:
		std::function<bool(QObject *, QKeyEvent *)> eventProcessor;

	public:
		GenericKeyEventHandler(QObject* parent = 0);
		virtual ~GenericKeyEventHandler();
		void setEventProcessor(std::function<bool(QObject *, QKeyEvent *)>);

	protected:
		bool eventFilter(QObject *obj, QEvent *event);

};



#endif // GENERICKEYEVENTHANDLER_HPP
