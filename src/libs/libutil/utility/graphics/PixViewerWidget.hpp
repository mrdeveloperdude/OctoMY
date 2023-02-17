#ifndef PixViewerWidget_HPP
#define PixViewerWidget_HPP

#include <QWidget>

class QImage;
class QPixmap;
namespace Ui
{
class PixViewerWidget;
}

class PixViewerWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PixViewerWidget *ui;
public:
	explicit PixViewerWidget(QWidget *parent = nullptr, QString name="PixViewerWidget");
	~PixViewerWidget();

public:
	void setImage(const QImage &im);
	void setPixmap(const QPixmap &px);

private slots:
	void on_pushButtonClose_clicked();

signals:
	void closeClicked();
};

#endif
// PixViewerWidget_HPP
