#ifndef MULTIVIEW_HPP
#define MULTIVIEW_HPP

#include <QWidget>

class MultiViewFilter;
class QAbstractButton;
class QAbstractItemModel;
class QItemSelectionModel;

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
	Ui::MultiView *ui{nullptr};
	QAbstractItemModel *mModel{nullptr};
	MultiViewFilter *mFilter{nullptr};
	QString k;
	QItemSelectionModel *mSelectionModel{nullptr};
	Settings *settings;
	double m_zoomFactor = 1.0;

public:
	const static QString DETAILS_KEY;
	const static QString GRID_KEY;
	const static QString LIST_KEY;
	const static QString TREE_KEY;
	
public:
	explicit MultiView(QWidget *parent = nullptr);
	~MultiView();
	
public:
	void configure(QSharedPointer<Settings> settings, QString val = LIST_KEY, QString key = QString());
	void setModel(QAbstractItemModel *data);
	QItemSelectionModel *selectionModel() const;
	
private:
	void setView(QString view);
	void setZoomFactor(double factor);

protected:
	// Override wheelEvent to capture Ctrl+scroll for zooming.
	void wheelEvent(QWheelEvent *event) override;

private slots:
	void handleDoubleClicked(const QModelIndex &index);
	void onViewButtonClicked(QAbstractButton*);
	void refreshView();
	void filterChanged(const QString &filter);

signals:
	void itemDoubleClicked(const QModelIndex &index);
};

#endif
// MULTIVIEW_HPP
