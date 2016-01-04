#include "StatsWindow.hpp"
#include "ui_StatsWindow.h"

StatsWindow::StatsWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::StatsWindow)
{
	srand(QDateTime::currentDateTime().toTime_t());
	ui->setupUi(this);


	// set locale to english, so we get english month names:
	ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
	double now = QDateTime::currentDateTime().toTime_t();

	ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes |
									QCP::iSelectLegend | QCP::iSelectPlottables);

	ui->customPlot->axisRect()->setupFullAxesBox();


	// configure bottom axis to show date and time instead of number:
	//ui->customPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
	// set a more compact font size for bottom and left axis tick labels:
	ui->customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
	ui->customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
	// set a fixed tick-step to one tick per month:
	ui->customPlot->xAxis->setAutoTickStep(true);
	//ui->customPlot->xAxis->setTickStep(1); // one tick per second ui->customPlot->xAxis->setSubTickCount(10);
	// apply manual tick and tick label for left axis:
	//ui->customPlot->yAxis->setAutoTicks(false); 	ui->customPlot->yAxis->setAutoTickLabels(false);
	//ui->customPlot->yAxis->setTickVector(QVector<double>() << 0 << 100); 	ui->customPlot->yAxis->setTickVectorLabels(QVector<QString>() << "0%" << "100%");
	// set axis labels:
	ui->customPlot->xAxis->setLabel("Seconds");

	ui->customPlot->xAxis->setRange(now, now+60);//One minute
	ui->customPlot->yAxis->setRange(-10, 110);


	ui->customPlot->yAxis->setLabel("%");
	ui->customPlot->legend->setVisible(true);
	QFont legendFont = font();
	legendFont.setPointSize(10);
	ui->customPlot->legend->setFont(legendFont);
	ui->customPlot->legend->setSelectedFont(legendFont);
	ui->customPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

	// connect slot that ties some axis selections together (especially opposite axes):
	connect(ui->customPlot, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
	// connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
	connect(ui->customPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
	connect(ui->customPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

	// make bottom and left axes transfer their ranges to top and right axes:
	connect(ui->customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui->customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->customPlot->yAxis2, SLOT(setRange(QCPRange)));

	// connect some interaction slots:
	connect(ui->customPlot, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
	connect(ui->customPlot, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
	connect(ui->customPlot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

	// connect slot that shows a message in the status bar when a graph is clicked:
	connect(ui->customPlot, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

	// setup policy and connect slot for context menu popup:
	ui->customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->customPlot, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

}



void StatsWindow::addGraph(QString legend , QCPDataMap &data, QPen pen){
	ui->customPlot->addGraph();
	ui->customPlot->graph()->setName(legend);
	ui->customPlot->graph()->setData(&data,false);
	ui->customPlot->graph()->setPen(pen);
	ui->customPlot->graph()->rescaleAxes();
	ui->customPlot->replot();
}


StatsWindow::~StatsWindow(){
	delete ui;
}

void StatsWindow::titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title){
	Q_UNUSED(event)
	// Set the plot title by double clicking on it
	bool ok;
	QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
	if (ok)
	{
		title->setText(newTitle);
		ui->customPlot->replot();
	}
}

void StatsWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part){
	// Set an axis label by double clicking on it
	if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
	{
		bool ok;
		QString newLabel = QInputDialog::getText(this, "QCustomPlot example", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
		if (ok)
		{
			axis->setLabel(newLabel);
			ui->customPlot->replot();
		}
	}
}

void StatsWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
	// Rename a graph by double clicking on its legend item
	Q_UNUSED(legend)
	if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
	{
		QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
		bool ok;
		QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
		if (ok)
		{
			plItem->plottable()->setName(newName);
			ui->customPlot->replot();
		}
	}
}

void StatsWindow::selectionChanged()
{
	/*
   normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
   the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
   and the axis base line together. However, the axis label shall be selectable individually.

   The selection state of the left and right axes shall be synchronized as well as the state of the
   bottom and top axes.

   Further, we want to synchronize the selection of the graphs with the selection state of the respective
   legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
   or on its legend item.
  */

	// make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
		ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		ui->customPlot->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
		ui->customPlot->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
	}
	// make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
	if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
		ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->customPlot->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
	{
		ui->customPlot->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
		ui->customPlot->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
	}

	// synchronize selection of graphs with selection of corresponding legend items:
	for (int i=0; i<ui->customPlot->graphCount(); ++i)
	{
		QCPGraph *graph = ui->customPlot->graph(i);
		QCPPlottableLegendItem *item = ui->customPlot->legend->itemWithPlottable(graph);
		if (item->selected() || graph->selected())
		{
			item->setSelected(true);
			graph->setSelected(true);
		}
	}
}

void StatsWindow::mousePress()
{
	// if an axis is selected, only allow the direction of that axis to be dragged
	// if no axis is selected, both directions may be dragged
	ui->checkBoxFollowData->setChecked(false);
	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->xAxis->orientation());
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeDrag(ui->customPlot->yAxis->orientation());
	else
		ui->customPlot->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void StatsWindow::mouseWheel()
{
	ui->checkBoxFollowData->setChecked(false);
	// if an axis is selected, only allow the direction of that axis to be zoomed
	// if no axis is selected, both directions may be zoomed

	if (ui->customPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->xAxis->orientation());
	else if (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
		ui->customPlot->axisRect()->setRangeZoom(ui->customPlot->yAxis->orientation());
	else
		ui->customPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}


void StatsWindow::removeSelectedGraph()
{
	if (ui->customPlot->selectedGraphs().size() > 0)
	{
		ui->customPlot->removeGraph(ui->customPlot->selectedGraphs().first());
		ui->customPlot->replot();
	}
}

void StatsWindow::removeAllGraphs()
{
	ui->customPlot->clearGraphs();
	ui->customPlot->replot();
}

void StatsWindow::contextMenuRequest(QPoint pos)
{
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (ui->customPlot->legend->selectTest(pos, false) >= 0) // context menu on legend requested
	{
		menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
		menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
		menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
		menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
		menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
	} else  // general context menu on graphs requested
	{
		if (ui->customPlot->selectedGraphs().size() > 0)
			menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
		if (ui->customPlot->graphCount() > 0)
			menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));
	}

	menu->popup(ui->customPlot->mapToGlobal(pos));
}

void StatsWindow::moveLegend()
{
	if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok)
		{
			ui->customPlot->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
			ui->customPlot->replot();
		}
	}
}

void StatsWindow::graphClicked(QCPAbstractPlottable *plottable)
{
	ui->statusBar->showMessage(QString("Clicked on graph '%1'.").arg(plottable->name()), 1000);
}


void StatsWindow::repaintPlot(){
	if(ui->checkBoxFollowData->isChecked()){
		if(ui->customPlot->graphCount()>0){
			const QCPRange range=ui->customPlot->xAxis->range();
			const double width=range.upper-range.lower;
			ui->customPlot->graph()->rescaleValueAxis();
			const double end=ui->customPlot->graph()->data()->lastKey();
			ui->customPlot->xAxis->setRange(end-width,end); //Look at last 60 sec
		}
	}
	ui->customPlot->replot();
}


