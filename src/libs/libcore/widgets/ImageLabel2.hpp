#ifndef IMAGELABEL2_HPP
#define IMAGELABEL2_HPP

#include <QLabel>

class ImageLabel2 : public QLabel
{
		Q_OBJECT
	private:

		QPixmap _qpCurrent;
		QPixmap _qpSource;
	public:
		explicit ImageLabel2(QWidget *parent = 0);

	private:
		void render();

	public:
		virtual void setPixmap(const QPixmap &);
		virtual void paintEvent(QPaintEvent *);


	signals:

	public slots:
};

#endif // IMAGEWIDGET_HPP
