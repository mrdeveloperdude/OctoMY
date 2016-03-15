#ifndef MAPEDITOR_HPP
#define MAPEDITOR_HPP

#include <QWidget>

namespace Ui {
	class MapEditor;
}

class MapEditor : public QWidget
{
		Q_OBJECT

	private:
		Ui::MapEditor *ui;

	public:
		explicit MapEditor(QWidget *parent = 0);
		~MapEditor();

	public:

		void prepareMap();
		void homeMap();

	private slots:
		void on_toolButtonHome_clicked();
};

#endif // MAPEDITOR_HPP
