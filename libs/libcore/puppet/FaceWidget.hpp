#ifndef FACEWIDGET_HPP
#define FACEWIDGET_HPP

#include <QWidget>
#include <QTimer>
#include <QVector2D>



class Eye{
	private:
		QVector2D center;
		QPolygonF basicEyeShapeBg;
		QPolygonF scaleraPolygon;
		QPolygonF irisPolygon;
		QPolygonF pupilPolygon;
		QPolygonF upperMask;
		QPolygonF lowerMask;
		QVector2D eyeSteer;
		QVector2D upperLidSteer;
		QVector2D lowerLidSteer;
		QVector2D squintSteer;
		qreal blink;
		qreal slant;

		QVector2D  specPos1;
		QVector2D  specPos2;
		bool dirty;
		QBrush lidBrush;
		QBrush scaleraBrush;
		QBrush irisBrush;
		QBrush pupilBrush;
		QBrush specularBrush;



	public:

		explicit Eye(QVector2D center, qreal slant);
		void update();
		void setBlink(qreal);
		void setExpression(QVector2D, QVector2D, QVector2D);
		void setSteer(QVector2D);
		void paint(QPainter &painter);
};


class FaceWidget : public QWidget
{
		Q_OBJECT
	private:
		QTimer timer;
		quint64 startTime;
		quint64 lastTime;
		qreal blink;
		qreal cycle;
		Eye leftEye;
		Eye rightEye;
		QBrush bgBrush;

		QVector2D upperLidSteer;
		QVector2D lowerLidSteer;
		QVector2D eyeSteer;
		QVector2D squintSteer;

		QVector2D lastPress;

	public:
		explicit FaceWidget(QWidget *parent = 0);
	protected:
		void paintEvent(QPaintEvent *);
		void hideEvent(QHideEvent *);
		void showEvent(QShowEvent *);
		void mousePressEvent(QMouseEvent *);
		void mouseMoveEvent(QMouseEvent *);
		void leaveEvent(QEvent *);

	public slots:
		void onUpdateTimer();
};

#endif // FACEWIDGET_HPP
