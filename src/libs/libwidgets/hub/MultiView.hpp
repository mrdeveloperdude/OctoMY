#ifndef MULTIVIEW_HPP
#define MULTIVIEW_HPP

#include <QWidget>

class QAbstractItemModel;
class QAbstractButton;

namespace Ui
{
class MultiView;
}

class Settings;

class MultiView : public QWidget
{
	Q_OBJECT
private:
	Ui::MultiView *ui;
	QAbstractItemModel *data ;
	QString k;
	Settings *settings;
	
public:
	explicit MultiView(QWidget *parent = nullptr);
	~MultiView();
	
public:
	void configure(QSharedPointer<Settings> settings, QString val, QString key);
	void setModel(QAbstractItemModel *data);
	
private:
	void setView(QString view);
private slots:
	void onViewButtonClicked(QAbstractButton*);

};

#endif
// MULTIVIEW_HPP
