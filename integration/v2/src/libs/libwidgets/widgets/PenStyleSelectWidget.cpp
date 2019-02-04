#include "PenStyleSelectWidget.hpp"

#include "uptime/MethodGate.hpp"

#include "uptime/ConnectionType.hpp"

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QPen>
#include <QPainter>
#include <QDebug>


PenStyleSelectWidget::PenStyleSelectWidget(QWidget *parent)
	: QWidget(parent)
	, mBasePen(QBrush (Qt::green), 2.5, Qt::SolidLine)
{
	OC_METHODGATE();
	setupUi();
}



PenStyleSelectWidget::~PenStyleSelectWidget()
{
	OC_METHODGATE();
}



void PenStyleSelectWidget::setupUi()
{
	OC_METHODGATE();
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(0);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	gridLayout->setContentsMargins(0, 0, 0, 0);

	//Pen Style
	{
		QComboBox *newComboBox = new QComboBox(this);
		newComboBox->setToolTip(newComboBox->tr("Line style"));
		newComboBox->setEditable(false);
		newComboBox->setIconSize(QSize(80,14));
		newComboBox->setMinimumWidth(80);
		if(!connect(newComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLineStyleSelected(int)), OC_CONTYPE)) {
			qDebug()<<"ERROR: could not connect";
		}

		for (int index = static_cast<int>(Qt::SolidLine); index < static_cast<int>(Qt::CustomDashLine); ++index) {
			QPixmap pix(80, 24);
			pix.fill(Qt::transparent);
			QPen pen(mBasePen);
			pen.setStyle(static_cast<Qt::PenStyle>(index));
			QPainter painter(&pix);
			painter.setPen(pen);
			painter.drawLine(2, 12, 78, 12);
			newComboBox->addItem(QIcon(pix), "");
		}

		newComboBox->setCurrentIndex(static_cast<int>(Qt::SolidLine) - 1);

		gridLayout->addWidget(newComboBox, 0, 0, 1, 1);

	}

}

void PenStyleSelectWidget::setBasePen(QPen basePen)
{
	OC_METHODGATE();
	mBasePen=basePen;
}

Qt::PenStyle PenStyleSelectWidget::penStyle()

{
	OC_METHODGATE();
	return  Qt::SolidLine;
}



void PenStyleSelectWidget::setPenStyle(Qt::PenStyle style)
{
	OC_METHODGATE();
	Q_UNUSED(style);
}
