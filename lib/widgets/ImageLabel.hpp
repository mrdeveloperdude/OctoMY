#ifndef IMAGELABEL_HPP
#define IMAGELABEL_HPP

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class ImageLabel : public QLabel
{
	Q_OBJECT
public:
	explicit ImageLabel(QWidget *parent = 0);
	virtual int heightForWidth( int width ) const;
	virtual QSize sizeHint() const;
signals:

public slots:
	void setPixmap ( const QPixmap & );
	void resizeEvent(QResizeEvent *);
private:
	QPixmap pix;
};

#endif // IMAGELABEL_HPP
