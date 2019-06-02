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
	QTimer mTimerUI;
	QTimer mTimerData;
	quint64 mBlinkStartTime;
	QPalette mPalette;
	int mUiUpdateInterval;
	int mDataUpdateInterval;
	QString mLastDataSignature;

public:
	explicit SimpleTableDebugWidget(QWidget *parent = nullptr);
	virtual ~SimpleTableDebugWidget();

public:
	void configure(QString name, int uiUpdateInterval = (1000/10) /* 10 fps */, int dataUpdateInterval= (2000) /* 0.5 FPS */);

public:
	void setData(QMap<QString, QString> data);
	void triggerUIBlink();

private slots:
	void onRealtimeChangedWrapper(bool realtime);
	void onUITimerWrapper();
	void onDataTimerWrapper();

private slots:
	void on_pushButtonUpdate_clicked();

public slots:
	virtual void onRealtimeChanged(bool realtime);
	virtual void onTimer();

};

#endif
// SIMPLETABLEDEBUGWIDGET_HPP
