#ifndef NAMEMAPPINGWIDGET_HPP
#define NAMEMAPPINGWIDGET_HPP

#include "puppet/NameMapping.hpp"

#include <QWidget>
#include <QMap>

class QVBoxLayout;
class PoseMapping;
class QButtonGroup;
class QAbstractButton;
class QHBoxLayout;

namespace Ui
{
class NameMappingWidget;
}

class NameMappingWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::NameMappingWidget *ui;
	NameMapping *mMapping;
	QStringList mFromList;
	QStringList mToList;
	QButtonGroup *mButGroupFrom;
	QButtonGroup *mButGroupTo;
	QList<QHBoxLayout *> mRows;

	QAbstractButton *mCurrentFromButton;
	QAbstractButton *mCurrentToButton;

	QSizePolicy mHumble;


public:
	explicit NameMappingWidget(QWidget *parent = 0);
	virtual ~NameMappingWidget();

public:

	void configure(NameMapping &mapping, QStringList fromList, QStringList toList);

	NameMapping *mapping();


	QStringList &fromList();
	QStringList &toList();
private:

	QHBoxLayout *row(quint32);

	void addFromButton(QString from, quint32 row);
	void addToButton(QString to, quint32 row);

	void makeConnection();

};

#endif // NAMEMAPPINGWIDGET_HPP
