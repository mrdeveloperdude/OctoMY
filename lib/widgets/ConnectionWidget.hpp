#ifndef CONNECTIONWIDGET_HPP
#define CONNECTIONWIDGET_HPP

#include <QWidget>

#include <QHostAddress>

#include "widgets/TryToggle.hpp"

namespace Ui {
	class ConnectionWidget;
}

class ConnectionWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit ConnectionWidget(QWidget *parent = 0);
		~ConnectionWidget();

	public:

		void configure(QString base);


		quint16 getLocalPort();
		QHostAddress getLocalAddress();
		quint16 getTargetPort();
		QHostAddress getTargetAddress();

		void setConnectState(TryToggleState);

		void hookSignals(QObject &ob);
		void unHookSignals(QObject &ob);


		void setEditsEnabled(bool e);

	private slots:
		void onConnectStateChanged(TryToggleState s);

	private:
		Ui::ConnectionWidget *ui;

	signals:

		void connectStateChanged(TryToggleState s);
};

#endif // CONNECTIONWIDGET_HPP
