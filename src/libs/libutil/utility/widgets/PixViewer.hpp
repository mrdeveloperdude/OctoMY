#ifndef PIXVIEWER_HPP
#define PIXVIEWER_HPP

#include <QWidget>

class QImage;
class QPixmap;
namespace Ui
{
class PixViewer;
}

class PixViewer : public QWidget
{
	Q_OBJECT
private:
	Ui::PixViewer *ui;
public:
	explicit PixViewer(QWidget *parent = 0, QString name="PixViewer");
	~PixViewer();

public:

	void setImage(const QImage &im);

	void setPixmap(const QPixmap &px);
private slots:
	void on_pushButtonClose_clicked();

signals:
	void closeClicked();
};

#endif // PIXVIEWER_HPP
