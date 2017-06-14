#ifndef PKIMANAGER_HPP
#define PKIMANAGER_HPP

#include <QWidget>

namespace Ui {
	class PKIManager;
}

class PKIManager : public QWidget
{
		Q_OBJECT

	public:
		explicit PKIManager(QWidget *parent = 0);
		~PKIManager();

	private:
		Ui::PKIManager *ui;
	public slots:

		void on_pushButtonGenerateKeyPair_clicked();
	private slots:
		void on_pushButtonSaveKeyPair_clicked();
		void on_pushButtonLoadKeyPair_clicked();
};

#endif // PKIMANAGER_HPP
