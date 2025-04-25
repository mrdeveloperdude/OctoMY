#ifndef COURIERSDEBUGWIDGET_H
#define COURIERSDEBUGWIDGET_H

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QWidget>
#include <QTimer>

class QTableWidgetItem;
class Node;
class Courier;

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
	QString setCourierTableItem(const int index, const QSharedPointer<Courier> courier);
	QString setCourierTableItem(const int index, const QString serviceName);
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
