#ifndef UTILITIESWIDGET_HPP
#define UTILITIESWIDGET_HPP

#include <QWidget>



class Hub;
class LogDevice;
class Settings;

namespace Ui {
class UtilitiesWidget;
}

class UtilitiesWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::UtilitiesWidget *ui;
	QSharedPointer<Hub> mHub;

public:
	explicit UtilitiesWidget(QWidget *parent = nullptr);
	~UtilitiesWidget();
private:
	
	QSharedPointer<Hub> hub();
public:
	void configure(QSharedPointer<Settings> settings, QSharedPointer<Hub> hub=nullptr);
	

	
};

#endif // UTILITIESWIDGET_HPP
