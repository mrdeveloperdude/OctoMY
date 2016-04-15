#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include <QWidget>
#include <QGeoPositionInfo>

namespace Ui {
	class MapEditor;
}

class QGeoPositionInfoSource;

class MapEditor : public QWidget
{
		Q_OBJECT

	private:
		Ui::MapEditor *ui;
		QGeoPositionInfoSource *gps;

	public:
		explicit MapEditor(QWidget *parent = 0);
		~MapEditor();

	public:

		void prepareMap();
		void homeMap();

	private slots:
		void onPositionUpdated(QGeoPositionInfo);
		void on_toolButtonHome_clicked();
		void on_toolButtonCenter_toggled(bool checked);
};

#endif // MAPEDITOR_HPP
