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
	explicit PixViewer(QWidget *parent = 0);
	~PixViewer();

public:

	void setImage(const QImage &im);

	void setPixmap(const QPixmap &px);
private slots:
	void on_pushButtonClose_clicked();


};

#endif // PIXVIEWER_HPP
