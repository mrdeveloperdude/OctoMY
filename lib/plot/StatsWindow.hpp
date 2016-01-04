#ifndef StatsWindow_H
#define StatsWindow_H

#include <QMainWindow>
#include <QInputDialog>
#include "qcustomplot.hpp"

namespace Ui {
	class StatsWindow;
}

class StatsWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit StatsWindow(QWidget *parent = 0);
		~StatsWindow();

	private slots:
		void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
		void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
		void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
		void selectionChanged();
		void mousePress();
		void mouseWheel();

		void removeSelectedGraph();
		void removeAllGraphs();
		void contextMenuRequest(QPoint pos);
		void moveLegend();
		void graphClicked(QCPAbstractPlottable *plottable);



	public slots:
		void addGraph(QString legend, QCPDataMap &data, QPen pen);
		void repaintPlot();
	private:
		Ui::StatsWindow *ui;
};

#endif // StatsWindow_H
