#include "PenWidthSelectWidget.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/ConnectionType.hpp"

#include <QComboBox>
#include <QDialog>
#include <QGridLayout>
#include <QPen>
#include <QPainter>
#include <QRect>

#include <QDebug>


PenWidthSelectWidget::PenWidthSelectWidget(QWidget *parent)
	: QWidget(parent)
	, combo(nullptr)
{
	OC_METHODGATE();
	setupUi();
	populateCombo();
}



PenWidthSelectWidget::~PenWidthSelectWidget()
{
	OC_METHODGATE();
}



void PenWidthSelectWidget::setupUi()
{
	OC_METHODGATE();
	QGridLayout *gridLayout = new QGridLayout(this);
	gridLayout->setSpacing(0);
	gridLayout->setObjectName(QStringLiteral("gridLayout"));
	gridLayout->setContentsMargins(0, 0, 0, 0);

	if(combo) {
		combo->deleteLater();
		combo=nullptr;
	}

	//Pen width
	if(nullptr == combo ) {
		combo = new QComboBox(this);
		combo->setToolTip(combo->tr("Line width"));
		combo->setEditable(false);
		combo->setMinimumWidth(80);
		if(!connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(penWidthChanged(qreal)), OC_CONTYPE)) {
			qWarning()<<"ERROR: could not connect";
		}
		gridLayout->addWidget(combo, 0, 0, 1, 1);
	}
}

void PenWidthSelectWidget::populateCombo()
{
	OC_METHODGATE();
	QGridLayout *gridLayout=static_cast<QGridLayout *>(layout());
	if(nullptr!=gridLayout) {

		//Pen width
		if(nullptr != combo ) {
			QSize size=sizeHint();
			combo->clear();
			combo->setIconSize(size);
			const qreal maximum= 10.0;
			const qreal step=1.0/2.0;
			for (qreal penWidth = step; penWidth < maximum; penWidth+=step) {
				QPixmap pix(static_cast<int>(maximum*3.0), static_cast<int>(maximum*2.0));
				pix.fill(Qt::white);

				QBrush brush(Qt::red);
				QPen pen(brush, static_cast<double>(penWidth), Qt::SolidLine);

				QPainter painter(&pix);
				painter.setPen(pen);
				painter.drawLine(static_cast<int>(maximum), static_cast<int>(maximum), static_cast<int>(pix.width()-maximum), static_cast<int>(maximum));

				combo->addItem(QIcon(pix),"");
			}

			combo->setCurrentIndex(0);
		}
	}
}

qreal PenWidthSelectWidget::penWidth()
{
	OC_METHODGATE();
	return 1.0;
}



void PenWidthSelectWidget::setPenWidth(qreal width)
{
	OC_METHODGATE();
	Q_UNUSED(width);
}



void PenWidthSelectWidget::resizeEvent(QResizeEvent *event)
{
	OC_METHODGATE();
	Q_UNUSED(event);
}
