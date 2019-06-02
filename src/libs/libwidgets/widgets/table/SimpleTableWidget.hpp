#ifndef SIMPLETABLEWIDGET_HPP
#define SIMPLETABLEWIDGET_HPP

#include <QTableView>

class Bundle
{

};

class SimpleTableModel;
class SimpleTableWidget : public QTableView
{
	Q_OBJECT
private:
	SimpleTableModel *model;
public:
	explicit SimpleTableWidget(QWidget *parent = nullptr);

public:
	void setData(QMap<QString, QString> &b);


};

#endif
// SIMPLETABLEWIDGET_HPP
