#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include <QWidget>

namespace Ui {
	class ConnectionManager;
}

class ConnectionManager : public QWidget
{
		Q_OBJECT

	public:
		explicit ConnectionManager(QWidget *parent = 0);
		~ConnectionManager();

	private slots:
		void on_pushButtonBack_clicked();

		void on_pushButtonPairing_clicked();

	private:
		Ui::ConnectionManager *ui;

	signals:
		void done();
		void startPairing();

};

#endif // CONNECTIONMANAGER_HPP
