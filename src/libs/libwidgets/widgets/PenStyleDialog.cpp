#include "PenStyleDialog.hpp"


#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include "PenWidthSelectWidget.hpp"
#include "PenStyleSelectWidget.hpp"

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QPen>
#include <QPainter>

QDialog *PenStyleDialog::dialog=nullptr;
PenStyleDialog *PenStyleDialog::penStyle=nullptr;

PenStyleDialog::PenStyleDialog(QWidget *parent)
	: QWidget(parent)
{
	OC_METHODGATE();
	setupUi();
}

PenStyleDialog::~PenStyleDialog()
{
	OC_METHODGATE();
}


void PenStyleDialog::setupUi()
{
	OC_METHODGATE();


	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(0);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	gridLayout->setContentsMargins(0, 0, 0, 0);




	//Pen Style
	{
		/*
		QComboBox *newComboBox = new QComboBox(this);
		newComboBox->setToolTip(newComboBox->tr("Line style"));
		newComboBox->setEditable(false);
		newComboBox->setIconSize(QSize(80,14));
		newComboBox->setMinimumWidth(80);
		connect(newComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotLineStyleSelected(int)));

		for (int aaa = (int)Qt::SolidLine; aaa < (int)Qt::CustomDashLine; ++aaa) {
			QPixmap pix(80, 14);

			pix.fill(Qt::white);

			QBrush brush(Qt::black);
			QPen pen(brush, 2.5, (Qt::PenStyle)aaa);

			QPainter painter(&pix);
			painter.setPen(pen);
			painter.drawLine(2, 7, 78, 7);

			newComboBox->addItem(QIcon(pix), "");
		}

		newComboBox->setCurrentIndex((int)Qt::SolidLine - 1);
		*/

		PenStyleSelectWidget *penStyleSelectWidget= OC_NEW PenStyleSelectWidget(this);

		gridLayout->addWidget(penStyleSelectWidget, 0, 0, 1, 1);

	}

	//Pen width
	{
		/*
		QComboBox *newComboBox = new QComboBox(this);
		newComboBox->setToolTip(newComboBox->tr("Line width"));
		newComboBox->setEditable(false);
		newComboBox->setIconSize(QSize(80,14));
		newComboBox->setMinimumWidth(80);
		connect(newComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(slotLineThicknessSelected(int)));

		for (int aaa = 1; aaa < 6; ++aaa) {
			QPixmap pix(80, 14);
			pix.fill(Qt::white);

			QBrush brush(Qt::black);
			QPen pen(brush, (double)aaa, Qt::SolidLine);

			QPainter painter(&pix);
			painter.setPen(pen);
			painter.drawLine(2, 7, 78, 7);

			newComboBox->addItem(QIcon(pix),"");
		}

		newComboBox->setCurrentIndex(0);
		*/

		PenWidthSelectWidget *penWidthSelectWidget= OC_NEW PenWidthSelectWidget(this);

		gridLayout->addWidget(penWidthSelectWidget, 0, 1, 1, 1);
	}
}


QPen PenStyleDialog::pen()
{
	OC_METHODGATE();
	return QPen();
}



QPen PenStyleDialog::getPen(QPen pen, QWidget *widget, QString title)
{
	OC_FUNCTIONGATE();
	Q_UNUSED(widget);
	Q_UNUSED(title);
	if(nullptr == dialog ) {
		dialog=new QDialog();
		dialog->setModal(true);
		QGridLayout *gridLayout = new QGridLayout(dialog);
		gridLayout->setSpacing(0);
		gridLayout->setObjectName(QStringLiteral("gridLayout"));
		gridLayout->setContentsMargins(0, 0, 0, 0);
		penStyle=new PenStyleDialog(dialog);
		gridLayout->addWidget(penStyle, 0, 0, 1, 1);
	}
	dialog->show();
	if(nullptr!=penStyle) {
		return penStyle->pen();
	}
	return pen;
}
