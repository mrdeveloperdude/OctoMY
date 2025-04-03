#ifndef MULTIVIEW_HPP
#define MULTIVIEW_HPP

#include <QWidget>

class QAbstractItemModel;
class QAbstractButton;
class MultiViewFilter;

namespace Ui
{
class MultiView;
}

class Settings;

// TODO: Move into widgets folder. Not particular to hub
class MultiView : public QWidget
{
	Q_OBJECT
private:
	Ui::MultiView *ui;
	QAbstractItemModel *data ;
	MultiViewFilter *mFilter;
	QString k;
	Settings *settings;
	double m_zoomFactor = 1.0;
	
public:
	explicit MultiView(QWidget *parent = nullptr);
	~MultiView();
	
public:
	void configure(QSharedPointer<Settings> settings, QString val, QString key);
	void setModel(QAbstractItemModel *data);
	
private:
	void setView(QString view);
	void setZoomFactor(double factor);

protected:
	// Override wheelEvent to capture Ctrl+scroll for zooming.
	void wheelEvent(QWheelEvent *event) override;

private slots:
	void onViewButtonClicked(QAbstractButton*);
	void refreshView();
	void filterChanged(const QString &filter);
};

#endif
// MULTIVIEW_HPP
