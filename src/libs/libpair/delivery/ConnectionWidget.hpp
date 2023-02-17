#ifndef CONNECTIONWIDGET_HPP
#define CONNECTIONWIDGET_HPP

#include "comms/NetworkAddress.hpp"
#include "components/TryToggleState.hpp"

#include <QWidget>
#include <QHostAddress>

namespace Ui {
	class ConnectionWidget;
}

class Settings;

class ConnectionWidget : public QWidget
{
		Q_OBJECT
	private:
		Ui::ConnectionWidget *ui;

		NetworkAddress mLocalAddress;
		NetworkAddress mRemoteAddress;

	public:
		explicit ConnectionWidget(QWidget *parent = nullptr);
		virtual ~ConnectionWidget();

	public:
		void configure(NetworkAddress &localAddress, NetworkAddress &remoteAddress);
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
