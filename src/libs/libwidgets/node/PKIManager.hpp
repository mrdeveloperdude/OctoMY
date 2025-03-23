#ifndef PKIMANAGER_HPP
#define PKIMANAGER_HPP

#include <QWidget>

namespace Ui {
	class PKIManager;
}

class PKIManager : public QWidget
{
	Q_OBJECT
private:
	Ui::PKIManager *ui;

public:
	explicit PKIManager(QWidget *parent = nullptr);
	~PKIManager();

public slots:
	void generateKey();
	void saveKey();
	void loadKey();
};

#endif // PKIMANAGER_HPP
