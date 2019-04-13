#include "app/style/StyleManager.hpp"
//#include "octostyle/OctoStyle.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"
#include "utility/file/File.hpp"

#include <QProxyStyle>
#include <QFontDatabase>
#include <QWidget>
#include <QDebug>
#include <QStyleFactory>
#include <QtPlugin>

AppStyle::AppStyle(QColor tinge)
	: mStyle(OC_NEW OctoMYProxyStyle("fusion"))
	, mPalette(palette(tinge))
	, mFonts{
	// App
	":/fonts/Dosis/Dosis-Regular.otf"
	, ":/fonts/Dosis/Dosis-Bold.otf"
	, ":/fonts/Dosis/Dosis-ExtraBold.otf"
	, ":/fonts/Dosis/Dosis-ExtraLight.otf"
	, ":/fonts/Dosis/Dosis-Light.otf"
	, ":/fonts/Dosis/Dosis-Medium.otf"
	, ":/fonts/Dosis/Dosis-SemiBold.otf"
	// LCD
	, ":/fonts/Other/digital-7 (mono).ttf"
	, ":/fonts/Other/code128.ttf"
}
, mDefaultFont("Dosis")

, mStyleSheet(""
			  // NOTE: This is disabled for now (it looks butt ugly)
			  // utility::file::fileToString(":/style/style.qss")
			 )
, mCursorFlashTime(500)
{
	OC_METHODGATE();
}


AppStyle::AppStyle(NodeType nodeType)
	: AppStyle(defaultColorForNodeType(nodeType))
{
	OC_METHODGATE();
}
AppStyle::~AppStyle()
{
	OC_METHODGATE();
}

void AppStyle::initializeResources()
{
	OC_METHODGATE();
	Q_INIT_RESOURCE(stylesheet);
	Q_INIT_RESOURCE(fonts);
	// Q_INIT_RESOURCE(OctoStyle);
}



QStringList AppStyle::fonts()
{
	OC_METHODGATE();
	return mFonts;
}

QString AppStyle::defaultFont()
{
	OC_METHODGATE();
	return mDefaultFont;
}


QIcon AppStyle::icon()
{
	OC_METHODGATE();
	QIcon icon(":/icons/octomy_logo_bare.svg");
	return icon;
}


QProxyStyle *AppStyle::style()
{
	OC_METHODGATE();
	//app.setStyle(OC_NEW OctoStyle);
	//app.setStyle(QStyleFactory::create("fusion"));
	return mStyle;
}


QPalette AppStyle::palette()
{
	OC_METHODGATE();
	return mPalette;
}


QString AppStyle::styleSheet()
{
	OC_METHODGATE();
	return mStyleSheet;
}

int AppStyle::cursorFlashTime()
{
	OC_METHODGATE();
	return mCursorFlashTime;
}





QPalette AppStyle::palette(QColor tinge)
{
	OC_METHODGATE();
	//Create dark theme palette with main colors adjusted to match the complementary hue of the tinge
	QPalette palette;
	const qreal hh= (tinge.hslHueF()-0.5);
	const qreal h = (hh<=0.0)?(hh+1.0):hh;
	const qreal s = 0.06;
	const QColor complementary=    QColor::fromHslF(qBound(0.0,h,1.0), qBound(0.0,tinge.hslSaturationF()*2.0,1.0), qBound(0.0,tinge.valueF(),1.0)).toRgb();
	const QColor lightest=QColor::fromHslF(h, s*8, 0.95,  1.0).toRgb();
	const QColor light=   QColor::fromHslF(h, s,   0.2,   1.0).toRgb();
	const QColor dark=    QColor::fromHslF(h, s*2, 0.05,  1.0).toRgb();
	const QColor darkest= QColor::fromHslF(h, s*8, 0.0125,1.0).toRgb();
	//qDebug()<<"COLORMIX: "<<hh<<" -> "<<h<<" tinge="<<tinge<<",  light="<<light<<",  dark="<<dark<<", ";
	palette.setColor(QPalette::Window, light);
	palette.setColor(QPalette::WindowText, lightest);
	palette.setColor(QPalette::Base, dark);
	palette.setColor(QPalette::AlternateBase, light);
	palette.setColor(QPalette::ToolTipBase, light);
	palette.setColor(QPalette::ToolTipText, lightest);
	palette.setColor(QPalette::Text, lightest);
	palette.setColor(QPalette::Button, light);
	palette.setColor(QPalette::ButtonText, lightest);
	palette.setColor(QPalette::BrightText, complementary);
	palette.setColor(QPalette::Highlight, tinge);
	palette.setColor(QPalette::HighlightedText, darkest);
	palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
	palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
	palette.setColor(QPalette::Link, QColor(42, 130, 218));
	return palette;
}


///////////////////////////////////////

OctoMYProxyStyle::OctoMYProxyStyle(QString key)
	: QProxyStyle(key)
	, OCTOMY_SLIDER_HANDLE_SIZE(35)
	, OCTOMY_CURSOR_WSIZE(3)
{
	OC_METHODGATE();
}

OctoMYProxyStyle::~OctoMYProxyStyle()
{
	OC_METHODGATE();
}

int OctoMYProxyStyle::pixelMetric ( PixelMetric metric, const QStyleOption * option, const QWidget * widget) const
{
	OC_METHODGATE();
	switch(metric) {
	case PM_SliderLength :
		return OCTOMY_SLIDER_HANDLE_SIZE;
	case PM_SliderThickness:
		return OCTOMY_SLIDER_HANDLE_SIZE;
	case PM_ScrollBarExtent:
		return OCTOMY_SLIDER_HANDLE_SIZE;
	case PM_ScrollBarSliderMin:
		return OCTOMY_SLIDER_HANDLE_SIZE;
	case PM_TextCursorWidth:
		return OCTOMY_CURSOR_WSIZE;
	default :
		return (QProxyStyle::pixelMetric(metric,option,widget));
	}
}



///////////////////////////////////////

StyleManager::StyleManager()
{
	OC_METHODGATE();
}


bool StyleManager::loadFont(QString name)
{
	OC_METHODGATE();
	const int fontId =	QFontDatabase::addApplicationFont ( name);
	if(fontId<0) {
		qWarning() << "ERROR: Could not load font for "<<name<< "( "<<fontId<<" )";
		return false;
	}
	return true;
}


void StyleManager::loadFonts(QStringList fonts)
{
	OC_METHODGATE();
	qputenv("QT_QPA_FONTDIR", "/");
	for(QString font:fonts) {
		loadFont(font);
	}
}


void StyleManager::setDefaultFont(QString fontName, QApplication &app)
{
	OC_METHODGATE();
	QFont font;
	font.setFamily(fontName);
	app.setFont(font);
}


void StyleManager::applyStyle(QProxyStyle *style, QApplication &app)
{
	OC_METHODGATE();
	app.setStyle(style);
}



void StyleManager::applyPalette(QPalette palette, QApplication &app)
{
	OC_METHODGATE();
	app.setPalette(palette);
}


void StyleManager::applyStyleSheet(QString styleSheet, QApplication &app)
{
	OC_METHODGATE();
	app.setStyleSheet (styleSheet);
}


void StyleManager::applyCursorFlashTime(int flashTime, QApplication &app)
{
	OC_METHODGATE();
	app.setCursorFlashTime(flashTime);
}


void StyleManager::apply(AppStyle &appStyle)
{
	OC_METHODGATE();
	QCoreApplication *capp=static_cast<QCoreApplication *>(QCoreApplication::instance());
	QApplication *app=qobject_cast<QApplication *>(capp);
	// GUI enabled
	if (nullptr!=app) {

		appStyle.initializeResources();

		QWidget *activeWindow=app->activeWindow();
		// Disable updates on active window to save user from unpleasant artifacts while the style is being changed.
		if(nullptr!=activeWindow) {
			activeWindow->setUpdatesEnabled(false);
		}
		app->setWindowIcon(appStyle.icon());
		loadFonts(appStyle.fonts());
		setDefaultFont(appStyle.defaultFont(), *app);
		applyStyle(appStyle.style(), *app);
		applyPalette(appStyle.palette(), *app);
		applyStyleSheet(appStyle.styleSheet(), *app);

		applyCursorFlashTime(appStyle.cursorFlashTime(), *app);


		if(nullptr!=activeWindow) {
			activeWindow->setUpdatesEnabled(true);
		}
	}
	// Headless (no GUI)
	else if(nullptr!=capp) {
		//qDebug()<<"No GUI, skipping styles";
	}
	// App not found
	else {
		qWarning()<<"ERROR: No application found while applying styles";
	}
}


void StyleManager::listStyles()
{
	OC_METHODGATE();
	QStringList availableStyles=QStyleFactory::keys();
	foreach (QString s, availableStyles) {
		qDebug()<<"STYLE: "<<s;
	}
}
