#ifndef QRWIDGET_HPP
#define QRWIDGET_HPP

#include <QWidget>

class QRWidget : public QWidget{
		Q_OBJECT
	private:
		QString data;
		QPixmap *m_doubleBuffer;
		QColor fg;
		QColor bg;
		bool dirty;

	public:
		explicit QRWidget(QWidget *parent = 0);
		void setQRData(QString data);

	protected:
		void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;


};

#endif // QRWIDGET_HPP
