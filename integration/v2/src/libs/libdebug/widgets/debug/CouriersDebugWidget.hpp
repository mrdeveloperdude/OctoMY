#ifndef COURIERSDEBUGWIDGET_HPP
#define COURIERSDEBUGWIDGET_HPP


#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QTimer>
#include <QSharedPointer>

class QTableWidgetItem;
class Node;

namespace Ui
{
class CouriersDebugWidget;
}

class CouriersDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::CouriersDebugWidget *ui;
private:
	ConfigureHelper mConfigureHelper;
	QTimer mTimer;
	quint64 mLastUpdate;
	QPalette mPalette;
	QSharedPointer<Node> mNode;
	QString mLastSig;

public:
	explicit CouriersDebugWidget(QWidget *parent = nullptr);
	virtual ~CouriersDebugWidget();

public:
	void configure(QSharedPointer <Node> node);
	void updateServiceTable();

private:
	void setServiceTableItem(const int index, const QString serviceName, const bool expected, const bool actual);
	QTableWidgetItem *tableItem(const bool s);
	QTableWidgetItem *tableItem(const QString s);
	void triggerUpdate();

private slots:
	void on_pushButtonUpdate_clicked();

private slots:
	void onCouriersChanged();
	void onRealtimeChanged(bool realtime);
	void onTimer();

};


#endif
// COURIERSDEBUGWIDGET_HPP
