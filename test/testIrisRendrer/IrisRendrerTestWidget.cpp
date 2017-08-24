#include "IrisRendrerTestWidget.hpp"
#include "ui_IrisRendrerTestWidget.h"

#include "basic/Settings.hpp"
#include <QPixmap>
#include <QImage>

#include <QDateTime>

IrisRendrerTestWidget::IrisRendrerTestWidget(QWidget *parent, QString name) :
	QWidget(parent),
	ui(new Ui::IrisRendrerTestWidget)
{
	ui->setupUi(this);
	setWindowTitle(name);

	Settings *s=new Settings("test","IrisRendrerTestWidget","1.0", this);
	for(int row=0; row<20; ++row) {
		QString name("val"+QString::number(row));
		QString *namep=&name;
		QLabel *label = new QLabel(ui->widget);
		label->setText(name);
		ui->gridLayout_2->addWidget(label, row, 0, 1, 1);

		QSlider *horizontalSlider = new QSlider(ui->widget);
		mSliders.append(horizontalSlider);
		horizontalSlider->setOrientation(Qt::Horizontal);
		ui->gridLayout_2->addWidget(horizontalSlider, row, 1, 1, 1);

		connect(horizontalSlider, &QSlider::valueChanged, this, [=](int position) {
			s->setCustomSettingLong(*namep,position);
			const qreal min=horizontalSlider->minimum();
			const qreal max=horizontalSlider->maximum();
			const qreal range=max-min;
			const qreal value=((position-min)/range);
			emit valueChanged(row, value);
		}

			   );

		horizontalSlider->setValue(s->getCustomSettingLong(*namep,0));
/*
		QTimer::singleShot(100, [=]() {
			long val=s->getCustomSettingLong(*namep,0);
			horizontalSlider->setValue(val);
		} );
		*/

	}

}

IrisRendrerTestWidget::~IrisRendrerTestWidget()
{
	delete ui;
	ui=nullptr;
}

void IrisRendrerTestWidget::setImage(const QImage &im)
{
	setPixmap(QPixmap ::fromImage(im));
}

void IrisRendrerTestWidget::setPixmap(const QPixmap &px)
{
	ui->label->setPixmap(px);
}


void IrisRendrerTestWidget::on_pushButtonClose_clicked()
{
	close();
	emit closeClicked();
}

void IrisRendrerTestWidget::on_pushButtonRandom_clicked()
{
	qsrand(QDateTime::currentMSecsSinceEpoch());
	auto l=mSliders.size();
	for(int row=0; row<l; ++row) {
		QSlider *horizontalSlider = mSliders[row];
		auto range=horizontalSlider->maximum()-horizontalSlider->minimum();
		auto val=horizontalSlider->minimum()+qrand()%range;
		horizontalSlider->setValue(val);
	}
}
