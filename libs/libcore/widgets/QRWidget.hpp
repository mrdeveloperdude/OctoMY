#ifndef QRWIDGET_HPP
#define QRWIDGET_HPP

#include <QWidget>

class QRWidget : public QWidget
{
	Q_OBJECT
private:
	QString mData;
	QPixmap mDoubleBuffer;
	QColor mFG;
	QColor mBG;
	bool mDirty;

public:
	explicit QRWidget(QWidget *parent = 0);
	void setQRData(QString mData);

public:

	const QPixmap &qrPixmap();

protected:
	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

signals:

	void qrRedraw();

};

#endif // QRWIDGET_HPP
