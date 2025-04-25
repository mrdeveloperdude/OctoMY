#include "WidgetIllustrationEditor.hpp"
#include "ui_WidgetIllustrationEditor.h"


#include "uptime/MethodGate.hpp"
#include "utility/graphics/WidgetCaptureTool.hpp"

#include <QFileDialog>
#include <QSharedPointer>

WidgetIllustrationEditor::WidgetIllustrationEditor(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::WidgetIllustrationEditor)
	, mConfigureHelper("WidgetIllustrationEditor", true, false, false, true)
{
	ui->setupUi(this);
}

WidgetIllustrationEditor::~WidgetIllustrationEditor()
{
	delete ui;
}

void WidgetIllustrationEditor::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()){
		ui->lineEditWidgetIlilustrationPath->setText(mWidgetIllustrationOutputDir.absolutePath());
		ui->widgetWidgetIllustrationPairing->configure(node);
		ui->widgetWidgetIllustrationIdenticon->configure(true);
		//ui->widgetWidgetIllustrationGait
		//ui->widgetWidgetIllustrationMapEditor
	}
}

void WidgetIllustrationEditor::on_pushButtonWidgetIllustrationBrowse_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mTopDir == mWidgetIllustrationOutputDir) {
			mWidgetIllustrationOutputDir=QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
		}
		QString dir=mWidgetIllustrationOutputDir.absolutePath();
		QString selectedDir = QFileDialog::getExistingDirectory(this, tr("Select Widget Illustration output dir"), dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
		if (selectedDir!=dir) {
			mWidgetIllustrationOutputDir=QDir(selectedDir);
			ui->lineEditWidgetIlilustrationPath->setText(mWidgetIllustrationOutputDir.absolutePath());
		}
	}
}


void WidgetIllustrationEditor::on_pushButtonWidgetIllustrationSaveAll_clicked()
{
	OC_METHODGATE();
	if(mConfigureHelper.isConfiguredAsExpected()){
		if(mWidgetIllustrationOutputDir.exists()) {
			WidgetCaptureTool wct(palette().color(QPalette::WindowText), palette().color(QPalette::Window), QColor(0x00, 0x00, 0x00, 0x08), QColor(0x00, 0x00, 0x00, 0x00));
			wct.captureWidget(ui->widgetWidgetIllustrationIdenticon, mWidgetIllustrationOutputDir, "identicon", "Identicon widget shows rendering of the 'personality' of your agent");
			wct.captureWidget(ui->widgetWidgetIllustrationGait, mWidgetIllustrationOutputDir, "gait", "Gait widget shows a 2D graphical representation of the current gait as it progresses");
			wct.captureWidget(ui->widgetWidgetIllustrationMapEditor, mWidgetIllustrationOutputDir, "map", "Map editor widget allows user top view or edit a feature on a map such as position, area or path.");
			wct.captureWidget(ui->widgetWidgetIllustrationPairing, mWidgetIllustrationOutputDir, "pairing", "Pairing wizard allows user to pair with discovered peers on the network");
		} else {
			qWarning()<<"ERROR: Output dir for widget illustration did not exist, aborting...";
		}
	}
}
