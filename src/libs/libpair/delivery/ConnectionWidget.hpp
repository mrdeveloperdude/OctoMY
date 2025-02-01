#ifndef CONNECTIONWIDGET_HPP
#define CONNECTIONWIDGET_HPP

#include "components/TryToggleState.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QHostAddress>
#include <QWidget>

namespace Ui {
	class ConnectionWidget;
}

class Settings;
class CarrierAddress;

class ConnectionWidget : public QWidget
{
		Q_OBJECT
	private:
		Ui::ConnectionWidget *ui;

		QSharedPointer<CarrierAddress> mLocalAddress;
		QSharedPointer<CarrierAddress> mRemoteAddress;

	public:
		explicit ConnectionWidget(QWidget *parent = nullptr);
		virtual ~ConnectionWidget();

	public:
		void configure(QSharedPointer<CarrierAddress> localAddress, QSharedPointer<CarrierAddress> remoteAddress);
		quint16 getLocalPort();
		QHostAddress getLocalAddress();
		quint16 getTargetPort();
		QHostAddress getTargetAddress();
		TryToggleState connectState();
		void setConnectState(TryToggleState);
		void hookSignals(QObject &ob);
		void unHookSignals(QObject &ob);
		void setEditsEnabled(bool e);

	private slots:
		void onConnectStateChanged(TryToggleState s);

	signals:
		void connectStateChanged(TryToggleState s);

};

#endif // CONNECTIONWIDGET_HPP
