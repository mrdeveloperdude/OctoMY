#ifndef STYLE_H
#define STYLE_H

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QMargins>
#include <QPainter>
#include <QPen>


QFont loadFont(QString source, int size, bool doDebug=false);
QColor mix(const QColor &a ,const QColor &b, qreal m);

const int selectionDarker{20};
const int fontSize{12};
const int sequenceTrackMargin{3};
const int sequenceGutterMargin{sequenceTrackMargin};

const QPainter::RenderHints standardRenderHints{QPainter::RenderHints()};

struct Style{
	const QBrush lassoBrush;
	const QColor graphBackground;
	const QColor nodeFill;
	const QColor nodeFillSelected;
	const QColor nodeOutline;
	const QColor nodeOutlineSelected;
	const QColor triFill;
	const QColor triFillSelected;
	const QColor triOutline;
	const QFont nodeFont;
	const QPainter::RenderHints renderHints;
	const QPen connectionPen;
	const QPen connectionSelectedPen;
	const QPen lassoPen;
	const QPen triOutlinePen;
	const QPen triOutlineSelectedPen;

	Style(
		  QPainter::RenderHints renderHints = standardRenderHints
		, QColor connection = QColor(0x222222)
		, QColor triFill = QColor(0xa9aec7)
		, QColor nodeFill = QColor(0xc6beaa)
		, QColor graphBackground = QColor(0xdad6c9)
		, QColor nodeOutlineTint = Qt::black
		, QColor nodeSelectTint = Qt::white
		, QColor triOutlineTint = Qt::black
		, QColor triSelectTint = Qt::white
		, QColor connectionSelected = QColor(0xffffff)
		, QColor triSelected = QColor(0xffff00)

		, float nodeOutlineTintAmount = 0.5
		, float nodeSelectTintAmount = 0.5

		, QFont nodeFont = loadFont(":/font/coolvetica.otf", fontSize)
		)
		:
		  lassoBrush(QColor(0, 0, 0, 20))
		, graphBackground(graphBackground)
		, nodeFill(nodeFill)
		, nodeFillSelected( mix(nodeFill, nodeSelectTint, nodeSelectTintAmount) )
		, nodeOutline( mix(nodeFill, nodeOutlineTint, nodeOutlineTintAmount) )
		, nodeOutlineSelected( mix(nodeOutline, nodeSelectTint, nodeSelectTintAmount) )
		, triFill(triFill)
		, triFillSelected( mix(triFill, triSelectTint, nodeSelectTintAmount) )
		, triOutline( mix(triFill, triOutlineTint, nodeOutlineTintAmount) )
		, nodeFont(nodeFont)
		, renderHints(renderHints)
		, connectionPen(connection, 2, Qt::PenStyle::DotLine)
		, connectionSelectedPen(connectionSelected, 2, Qt::PenStyle::DotLine)
		, lassoPen(QColor(255, 255, 255, 20), 2.0, Qt::PenStyle::DashLine)
		, triOutlinePen(triOutline, 1,Qt::PenStyle::SolidLine)
		, triOutlineSelectedPen(triSelected, 1, Qt::PenStyle::DotLine)
	{}
	
};

#endif // STYLE_H
