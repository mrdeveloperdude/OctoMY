#ifndef STYLEMANAGER_HPP
#define STYLEMANAGER_HPP

#include "node/NodeType.hpp"

#include <QString>
#include <QApplication>
#include <QColor>
#include <QProxyStyle>



/**
 * @brief OctoMYProxyStyle is a class that is used to extend the existing standard "fusion" style in Qt with some changes.
 * Most notably it changes the slider handle size to be more useful for touch enabled applications.
 *
 */
class OctoMYProxyStyle : public QProxyStyle
{
private:
	const int OCTOMY_SLIDER_HANDLE_SIZE;
	const int OCTOMY_CURSOR_WSIZE;

public:
	explicit OctoMYProxyStyle(QString key);
	virtual ~OctoMYProxyStyle();

public:
	int pixelMetric ( PixelMetric metric, const QStyleOption * option = nullptr, const QWidget * widget = nullptr ) const;
};



/**
 * @brief The AppStyle class represents one set of styles to be managed by StyleManager.
 * It has a color palette, a list of fonts, a stylesheet and other style related stuff
 * all baked into one convenient package.
 */

class AppStyle
{

private:
	QProxyStyle *mStyle;
	QPalette mPalette;
	QStringList mFonts;
	QString mDefaultFont;
	QString mStyleSheet;
	int mCursorFlashTime{500};
	bool mResourcesInitialized{false};

public:
	explicit AppStyle(QColor tinge = QColor(142, 45, 197).lighter());
	explicit AppStyle(NodeType nodeType);
	virtual ~AppStyle();

protected:
	QPalette palette(QColor tinge);

public:
	void initializeResources();

	QStringList fonts();
	QString defaultFont();

	QIcon icon();
	QProxyStyle *style();
	QPalette palette();
	QString styleSheet();
	int cursorFlashTime();

};



/**
 * @brief The StyleManager class is a convenience wrapper around Qt's own style managent classes.
 * It is used to manage the style of an app.
 * It has facilities for loading custom fonts and applying custom style sets.
 */
class StyleManager
{
private:
	bool mDebug{false};

public:
	StyleManager();

private:
	bool loadFont(QString name);
	void loadFonts(QStringList fonts);
	void setDefaultFont(QString font, QApplication &app);
	void applyStyle(QProxyStyle *style, QApplication &app);
	void applyPalette(QPalette palette, QApplication &app);
	void applyStyleSheet(QString styleSheet, QApplication &app);
	void applyCursorFlashTime(int flashTime, QApplication &app);
	
	void setDefaultFont(QString font, QWidget &widget);
	void applyStyle(QProxyStyle *style, QWidget &widget);
	void applyPalette(QPalette palette, QWidget &widget);
	void applyStyleSheet(QString styleSheet, QWidget &widget);

public:
	// Apply style application wide
	void apply(AppStyle &appStyle);
	// Apply style for single window/widget
	void apply(AppStyle &appStyle, QWidget &widget);
	void listStyles();
};



#endif
// STYLEMANAGER_HPP
