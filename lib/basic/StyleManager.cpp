#include "basic/StyleManager.hpp"
#include "basic/Standard.hpp"
#include "utility/Utility.hpp"

#include <QProxyStyle>
#include <QFontDatabase>
#include <QWidget>
#include <QDebug>

class OctoMYProxyStyle : public QProxyStyle{
		const qint64 OCTOMY_SLIDER_HANDLE_SIZE=35;
		const qint64 OCTOMY_CURSOR_WSIZE=3;
	public:
		int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const{
			WWMETHODGATE();
			switch(metric) {
				case PM_SliderLength : return OCTOMY_SLIDER_HANDLE_SIZE;
				case PM_SliderThickness: return OCTOMY_SLIDER_HANDLE_SIZE;
				case PM_ScrollBarExtent: return OCTOMY_SLIDER_HANDLE_SIZE;
				case PM_ScrollBarSliderMin: return OCTOMY_SLIDER_HANDLE_SIZE;
				case PM_TextCursorWidth: return OCTOMY_CURSOR_WSIZE;
				default : return (QProxyStyle::pixelMetric(metric,option,widget));
			}
		}
};


///////////////////////////////////////

StyleManager::StyleManager(){
	WWMETHODGATE();
}

void StyleManager::apply(){
	WWMETHODGATE();
	QCoreApplication *capp=static_cast<QCoreApplication *>(QCoreApplication::instance());
	QApplication *app=qobject_cast<QApplication *>(capp);
	//GUI enabled
	if (0!=app) {
		QWidget *w=app->activeWindow();
		if(0!=w){
			w->setUpdatesEnabled(false);
		}
		QIcon icon(":/icons/spider.svg");
		app->setWindowIcon(icon);
		loadFonts(*app);
		applyStyle(*app);
		applyStyleSheet(*app);
		if(0!=w){
			w->setUpdatesEnabled(true);
		}
	}
	//GUI not enabled
	else if(0!=capp){
		//qDebug()<<"No GUI, skipping styles";
	}
}

bool StyleManager::loadFont(QString name){
	WWMETHODGATE();
	const int fontId =	QFontDatabase::addApplicationFont ( name);
	if(fontId<0){
		qWarning() << "ERROR: Could not load font for "<<name<< "( "<<fontId<<" )";
		return false;
	}
	return true;
}




void StyleManager::loadFonts(QApplication &app){
	WWMETHODGATE();
	qputenv("QT_QPA_FONTDIR", "/");
	//Main
	loadFont (":/fonts/CenturyGothicStd.otf");
	loadFont (":/fonts/CenturyGothicStd-Bold.otf");
	loadFont (":/fonts/CenturyGothicStd-Italic.otf");
	loadFont (":/fonts/CenturyGothicStd-BoldItalic.otf");
	//LCD
	loadFont (":/fonts/digital-7 (mono).ttf");
	//Set standard font
	QFont font;
	font.setFamily("CenturyGothicStd");
	app.setFont(font);
}

void StyleManager::applyStyle(QApplication &app){
	WWMETHODGATE();
	//DELETE 2014-09-26
	app.setStyle(new OctoMYProxyStyle);
	app.setCursorFlashTime(500);
}


void StyleManager::applyStyleSheet(QApplication &app){
	WWMETHODGATE();
	app.setStyleSheet ( utility::fileToString(":/style/style.qss"));
}


