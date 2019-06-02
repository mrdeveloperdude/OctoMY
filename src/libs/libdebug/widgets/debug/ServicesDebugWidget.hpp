#ifndef SERVICESDEBUGWIDGET_HPP
#define SERVICESDEBUGWIDGET_HPP

#include "uptime/ConfigureHelper.hpp"

#include <QWidget>
#include <QTimer>
#include <QSharedPointer>

class QTableWidgetItem;
class Node;

namespace Ui
{
class ServicesDebugWidget;
}

class ServicesDebugWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::ServicesDebugWidget *ui;
private:
	ConfigureHelper mConfigureHelper;
	QTimer mTimer;
	quint64 mLastUpdate;
	QPalette mPalette;
	QSharedPointer<Node> mNode;
	QString mLastSig;

public:
	explicit ServicesDebugWidget(QWidget *parent = nullptr);
	virtual ~ServicesDebugWidget();

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
	void onServicesChanged();
	void onRealtimeChanged(bool realtime);
	void onTimer();


};

#endif
// SERVICESDEBUGWIDGET_HPP
