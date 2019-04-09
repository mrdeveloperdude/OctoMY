#ifndef SIMPLETABLEDEBUGWIDGET_HPP
#define SIMPLETABLEDEBUGWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"


#include <QWidget>
#include <QTimer>

namespace Ui
{
class SimpleTableDebugWidget;
}

class SimpleTableDebugWidget : public QWidget
{
	Q_OBJECT
protected:
	Ui::SimpleTableDebugWidget *ui;
private:
	ConfigureHelper mConfigureHelper;
	QTimer mTimer;
	quint64 mLastUpdate;
	QPalette mPalette;

public:
	explicit SimpleTableDebugWidget(QWidget *parent = nullptr);
	virtual ~SimpleTableDebugWidget();

public:
	void configure(QString name);

public:
	void setData(QMap<QString, QString> data);

private slots:
	void onRealtimeChangedWrapper(bool realtime);
	void onTimerWrapper();

	void on_pushButtonUpdate_clicked();

public slots:
	virtual void onRealtimeChanged(bool realtime);
	virtual void onTimer();

};

#endif
// SIMPLETABLEDEBUGWIDGET_HPP
