#ifndef STREAMCODECWIDGET_HPP
#define STREAMCODECWIDGET_HPP

#include <QWidget>


class QImage;
class QPixmap;
namespace Ui
{
class StreamCodecWidget;
}

class StreamCodecWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::StreamCodecWidget *ui;


public:
	explicit StreamCodecWidget(QWidget *parent = nullptr);
	~StreamCodecWidget();


public:
	void setImage(const QImage &im, quint8 num);
	void setPixmap(const QPixmap &px, quint8 num);

signals:
	void trigger();


private slots:
	void on_pushButtonTrigger_clicked();


};

#endif // STREAMCODECWIDGET_HPP
