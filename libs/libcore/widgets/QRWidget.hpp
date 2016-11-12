#ifndef QRWIDGET_HPP
#define QRWIDGET_HPP

#include <QWidget>

class QRWidget : public QWidget
{
	Q_OBJECT
private:
	QString data;
	QPixmap m_doubleBuffer;
	QColor fg;
	QColor bg;
	bool dirty;

public:
	explicit QRWidget(QWidget *parent = 0);
	void setQRData(QString data);

public:

	const QPixmap &qrPixmap();

protected:
	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

signals:

	void qrRedraw();

};

#endif // QRWIDGET_HPP
