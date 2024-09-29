#ifndef UTILITIESWIDGET_HPP
#define UTILITIESWIDGET_HPP

#include <QWidget>
#include <QHostInfo>


class Hub;
class LogDevice;
class Settings;
class ZBarScanner;

namespace Ui {
class UtilitiesWidget;
}

class UtilitiesWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::UtilitiesWidget *ui;
	QSharedPointer<Hub> mHub;
	ZBarScanner *mScanner;
	LogDevice *mLogDevice{nullptr};

	
public:
	explicit UtilitiesWidget(QWidget *parent = nullptr);
	~UtilitiesWidget();
private:
	
	QSharedPointer<Hub> hub();
public:
	void configure(QSharedPointer<Settings> settings, QSharedPointer<Hub> hub=nullptr);
	
private slots:
	void onQRRedraw();
	void on_lineEditQR_textChanged(const QString &arg1);
	void onRemoteHostLookupComplete(QHostInfo hi);
	
};

#endif // UTILITIESWIDGET_HPP
