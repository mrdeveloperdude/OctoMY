#ifndef SERVICESDEBUGWIDGET_HPP
#define SERVICESDEBUGWIDGET_HPP

#include <QWidget>
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
	QSharedPointer<Node> mNode;

public:
	explicit ServicesDebugWidget(QWidget *parent = nullptr);
	virtual ~ServicesDebugWidget();

public:
	void configure(QSharedPointer <Node> node);
	void updateServiceTable();

	private slots:
	void on_pushButtonUpdate_clicked();

	private:
	void setServiceTableItem(const int index, const QString serviceName, const bool expected, const bool actual);
	QTableWidgetItem *tableItem(const bool s);
	QTableWidgetItem *tableItem(const QString s);

};

#endif
// SERVICESDEBUGWIDGET_HPP
