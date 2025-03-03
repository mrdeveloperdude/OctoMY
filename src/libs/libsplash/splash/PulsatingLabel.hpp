#ifndef PULSATINGLABEL_HPP
#define PULSATINGLABEL_HPP

#include <QLabel>

class QSvgRenderer;

class PulsatingLabel : public QLabel {
	Q_OBJECT
private:
	qreal m_scaleFactor;
	QSvgRenderer *m_svgRenderer;

	Q_PROPERTY(qreal scaleFactor READ scaleFactor WRITE setScaleFactor NOTIFY scaleFactorChanged)
public:
	explicit PulsatingLabel(QWidget *parent = nullptr);
	
	qreal scaleFactor() const;
	void setScaleFactor(qreal factor);
	
signals:
	void scaleFactorChanged(qreal);
	
protected:
	void paintEvent(QPaintEvent *event) override;


public:
	void setSvg(const QString &svgPath);

};

#endif // PULSATINGLABEL_HPP
