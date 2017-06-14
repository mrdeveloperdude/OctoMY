#ifndef SVGWIDGET_HPP
#define SVGWIDGET_HPP

#include <QtWidgets/QWidget>
#include <QtSvg/QSvgRenderer>

class SvgWidget : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(QString svgURL READ svgURL WRITE setSvgURL DESIGNABLE true)
	Q_PROPERTY(bool silhouette READ  silhouetteEnabled WRITE setSilhouetteEnabled DESIGNABLE true)
	Q_PROPERTY(QColor silhouetteForeground READ silhouetteForeground WRITE setSilhouetteForeground DESIGNABLE true)
	Q_PROPERTY(QColor silhouetteBackground READ silhouetteBackground WRITE setSilhouetteBackground DESIGNABLE true)
protected:
	QSvgRenderer *mSVG;
	QString mLastURL;
	QPixmap mDoubleBuffer;
	bool mSilhouetteEnabled;
	const QColor mTransparentColor;
	QColor mSilhouetteForeground;
	QColor mSilhouetteBackground;
	const QColor mFailColor;

	bool mDirty;

public:
	explicit SvgWidget(QWidget *parent = 0);
	virtual ~SvgWidget();

	void configure(bool doSilhouette=true, QColor silhouetteForeground=QColor("White"), QColor silhouetteBackground=QColor("#00000000"));

	void setSvg(QSvgRenderer &mSVG);

	void setSvgURL(QString url);
	QString svgURL() const;

	void setSilhouetteEnabled(bool);
	bool silhouetteEnabled() const;

	void setSilhouetteForeground(QColor );
	QColor  silhouetteForeground() const;

	void setSilhouetteBackground(QColor);
	QColor silhouetteBackground() const;


protected:
	void paintEvent(QPaintEvent *);

};

#endif // SVGWIDGET_HPP
