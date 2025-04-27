#include "ConstructShell.hpp"
#include "ui_ConstructShell.h"

#include <QLabel>
#include <QFileDialog>

ConstructShell::ConstructShell(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::ConstructShell)
{
	ui->setupUi(this);
	QObject::connect(ui->pushButtonGrabFrame, &QPushButton::clicked, ui->pushButtonGrabFrame, [=] {
		QImage image = ui->widgetRhi->grabFramebuffer();
		qDebug() << "Got image" << image;
		if (!image.isNull()) {
			QFileDialog fd(ui->widgetRhi->parentWidget());
			fd.setAcceptMode(QFileDialog::AcceptSave);
			fd.setDefaultSuffix("png");
			fd.selectFile("test.png");
			if (fd.exec() == QDialog::Accepted) {
				const auto files = fd.selectedFiles();
				if (!files.isEmpty()) {
					image.save(files.first());
				}
			}
		}
	});
	QObject::connect(ui->checkBoxAntialias, &QCheckBox::checkStateChanged, ui->checkBoxAntialias, [=] {
		if (ui->checkBoxAntialias->isChecked()){
			ui->widgetRhi->setSampleCount(4);
		}
		else{
			ui->widgetRhi->setSampleCount(1);
		}
	});
	auto HUDLabel = hookHUD();
	QObject::connect(ui->checkBoxHUD, &QCheckBox::checkStateChanged, ui->checkBoxHUD, [=] {
		if (ui->checkBoxHUD->isChecked()){
			HUDLabel->setVisible(true);
		}
		else{
			HUDLabel->setVisible(false);
		}
	});
}

ConstructShell::~ConstructShell()
{
	delete ui;
}


QWidget *ConstructShell::hookHUD(){
	auto HUDLabel = new QLabel(ui->widgetRhi);
	HUDLabel->setText(QObject::tr("This is a\nsemi-transparent\n overlay widget\n"
									  "placed on top of\nthe QRhiWidget."));
	HUDLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	HUDLabel->setAutoFillBackground(true);
	QPalette semiTransparent(QColor(255, 0, 0, 64));
	semiTransparent.setBrush(QPalette::Text, Qt::white);
	semiTransparent.setBrush(QPalette::WindowText, Qt::white);
	HUDLabel->setPalette(semiTransparent);
	QFont f = HUDLabel->font();
	f.setPixelSize(QFontInfo(f).pixelSize() * 2);
	f.setWeight(QFont::Bold);
	HUDLabel->setFont(f);
	HUDLabel->resize(320, 320);
	HUDLabel->hide();
	QObject::connect(ui->widgetRhi, &ConstructWidget::resized, ui->widgetRhi, [=] {
		HUDLabel->setGeometry(HUDLabel->geometry());
	});
	return HUDLabel;
}

