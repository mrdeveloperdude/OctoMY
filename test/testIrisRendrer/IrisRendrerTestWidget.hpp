#ifndef IRISRENDRERTESTWIDGET_HPP
#define IRISRENDRERTESTWIDGET_HPP

#include <QWidget>
#include <QList>

class QImage;
class QPixmap;
class QSlider;

namespace Ui
{
class IrisRendrerTestWidget;
}

class IrisRendrerTestWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::IrisRendrerTestWidget *ui;
	QList<QSlider *> mSliders;
public:
	explicit IrisRendrerTestWidget(QWidget *parent = 0, QString name="IrisRendrerTestWidget");
	~IrisRendrerTestWidget();

public:
	void setImage(const QImage &im);
	void setPixmap(const QPixmap &px);
private slots:
	void on_pushButtonClose_clicked();
	void on_pushButtonRandom_clicked();

signals:
	void closeClicked();
	void valueChanged(quint32 id, qreal value);
};

#endif // IRISRENDRERTESTWIDGET_HPP
