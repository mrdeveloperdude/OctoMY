#ifndef METABLOBSWIDGET_HPP
#define METABLOBSWIDGET_HPP


#include <QWidget>
#include <QTimer>
#include <QVector3D>
#include <QColor>
#include <QImage>

struct Blob {
	QVector3D position;
	float density;
	float r, g, b; // RGB values
	float shininess;
};

class MetaBlobsWidget : public QWidget
{
	Q_OBJECT
	
public:
	explicit MetaBlobsWidget(QWidget *parent = nullptr);
	~MetaBlobsWidget();
private:
	QTimer *animationTimer;
	QImage *canvasFront;
	QImage *canvasBack;
	QVector<Blob> blobs;
	float time;
	
protected:
	void paintEvent(QPaintEvent *) override;
	void resizeEvent(QResizeEvent *event) override;
	
	
private:
	inline void setupBlobs();
	inline void updateBlobsPosition();
	inline float computeField(const QVector3D &point);
	inline void renderMetablobs();
	inline QVector3D computeNormal(const QVector3D &point);
	inline QVector3D projectTo2D(const QVector3D &point);
};

#endif // METABLOBSWIDGET_HPP
