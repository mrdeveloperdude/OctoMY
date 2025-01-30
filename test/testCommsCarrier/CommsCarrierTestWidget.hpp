#ifndef COMMSCARRIERTESTWIDGET_HPP
#define COMMSCARRIERTESTWIDGET_HPP

#include <QWidget>

namespace Ui {
class CommsCarrierTestWidget;
}

class CommsCarrierTestWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::CommsCarrierTestWidget *ui;

public:
	explicit CommsCarrierTestWidget(QWidget *parent = nullptr);
	~CommsCarrierTestWidget();

public slots:
	void log(const QString &sender,const QString &message);
	
	void configureA();
	void configureB();
	void connectA(bool con);
	void connectB(bool con);
	void activateA(bool con);
	void activateB(bool con);
	void intervalChangeA(int interval);
	void intervalChangeB(int interval);

signals:
	void configureAImpl();
	void configureBImpl();
	void connectAImpl(bool con);
	void connectBImpl(bool con);
	void activateAImpl(bool con);
	void activateBImpl(bool con);
	void intervalChangeAImpl(int interval);
	void intervalChangeBImpl(int interval);

};

#endif // COMMSCARRIERTESTWIDGET_HPP
