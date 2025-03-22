#include "ScreenSelector.hpp"
#include "ui_ScreenSelector.h"

#include <QScreen>
#include <QGuiApplication>

ScreenSelector::ScreenSelector(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::ScreenSelector)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
	setAttribute(Qt::WA_TranslucentBackground);
	ui->setupUi(this);
}

ScreenSelector::~ScreenSelector()
{
	delete ui;
}

void ScreenSelector::setScreen(QScreen *screen)
{
	mScreen = screen;
	const auto screenValid=nullptr!=screen;
	// Hide all other information labels.
	ui->labelManufacturer->setVisible(screenValid);
	ui->labelModel->setVisible(screenValid);
	ui->labelSerialNumber->setVisible(screenValid);
	ui->labelResolution->setVisible(screenValid);
	ui->labelResolutionHeader->setVisible(screenValid);
	ui->labelAvailableResolution->setVisible(screenValid);
	ui->labelAvailableResolutionHeader->setVisible(screenValid);
	ui->labelRefreshRate->setVisible(screenValid);
	ui->labelRefreshRateHeader->setVisible(screenValid);
	ui->labelPhysicalSize->setVisible(screenValid);
	ui->labelPhysicalSizeHeader->setVisible(screenValid);
	ui->labelLogicalDPI->setVisible(screenValid);
	ui->labelLogicalDPIHeader->setVisible(screenValid);
	ui->labelPhysicalDPI->setVisible(screenValid);
	ui->labelPhysicalDPIHeader->setVisible(screenValid);
	ui->labelColorDepth->setVisible(screenValid);
	ui->labelColorDepthHeader->setVisible(screenValid);
	ui->labelPrimary->setVisible(screenValid);
	ui->labelPrimaryHeader->setVisible(screenValid);
	
	if(!screenValid){
		ui->labelScreenName->setText("No Screen");
		return;
	}
	
	// Screen name.
	ui->labelScreenName->setText(screen->name());
	
	// Manufacturer, Model, Serial Number.
	ui->labelManufacturer->setText(screen->manufacturer());
	ui->labelModel->setText(screen->model());
	ui->labelSerialNumber->setText(screen->serialNumber());
	
	// Resolution (full geometry).
	QRect geom = screen->geometry();
	ui->labelResolution->setText(QString("%1 x %2").arg(geom.width()).arg(geom.height()));
	
	// Available resolution.
	QRect avail = screen->availableGeometry();
	ui->labelAvailableResolution->setText(QString("%1 x %2").arg(avail.width()).arg(avail.height()));
	
	// Refresh rate.
	ui->labelRefreshRate->setText(QString("%1 Hz").arg(screen->refreshRate(), 0, 'f', 1));
	
	// Physical size in millimeters.
	QSizeF phys = screen->physicalSize();
	ui->labelPhysicalSize->setText(QString("%1 x %2 mm")
									   .arg(phys.width(), 0, 'f', 0).arg(phys.height(), 0, 'f', 0));
	
	// Logical DPI.
	ui->labelLogicalDPI->setText(QString("%1 DPI").arg(screen->logicalDotsPerInch(), 0, 'f', 1));
	
	// Physical DPI.
	ui->labelPhysicalDPI->setText(QString("%1 DPI").arg(screen->physicalDotsPerInch(), 0, 'f', 1));
	
	// Color depth.
	ui->labelColorDepth->setText(QString("%1-bit").arg(screen->depth()));
	
	// Primary screen indicator.
	bool isPrimary = (screen == QGuiApplication::primaryScreen());
	ui->labelPrimary->setText(isPrimary ? "Yes" : "No");
	int b{30};
	auto g = screen->geometry();
	g.adjust(-b, -b, b , b);
	ui->frame->setStyleSheet("QFrame{background-color: rgba(88, 66, 77, 0.75);}");
	setGeometry(g);
}


void ScreenSelector::selectScreen(){
	hide();
	emit screenSelected(mScreen);
}
