#ifndef CLIENTWINDOW_HPP
#define CLIENTWINDOW_HPP

#include <QWidget>
#include <QTimer>

#include "comms/Client.hpp"

namespace Ui {
	class ClientWindow;
}

class ClientWindow : public QWidget{
		Q_OBJECT

	private:
		Ui::ClientWindow *ui;
		Client *client;
		QTimer summaryTimer;
	public:
		explicit ClientWindow(Client *c, QWidget *parent = 0);
		~ClientWindow();

	public slots:
		void onSummaryTimer();
};

#endif // CLIENTWINDOW_HPP
