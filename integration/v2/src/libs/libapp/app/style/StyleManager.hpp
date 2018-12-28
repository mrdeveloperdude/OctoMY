#ifndef STYLEMANAGER_HPP
#define STYLEMANAGER_HPP

#include <QString>
#include <QApplication>
#include <QColor>
#include <QProxyStyle>



/**
 * @brief The OctoMYProxyStyle class is a class that is used to extend the existing style in Qt with some changes.
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

	int pixelMetric ( PixelMetric metric, const QStyleOption * option = nullptr, const QWidget * widget = nullptr ) const;
};




class AppStyle
{

private:
	QProxyStyle *mStyle;
	QPalette mPalette;
	QStringList mFonts;
	QString mDefaultFont;
	QString mStyleSheet;
	int mCursorFlashTime;

public:

	explicit AppStyle(QColor tinge=QColor(142,45,197).lighter());
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
 * @brief The StyleManager class is a convenience wrapper around Qt's owhn style managent classes.
 * It is used to manage the style of an app.
 * It hasfacilities for loading custom fonts and applying custom style sets.
 */
class StyleManager
{
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

public:
	void apply(AppStyle &appStyle);
	void listStyles();
};



#endif // STYLEMANAGER_HPP
