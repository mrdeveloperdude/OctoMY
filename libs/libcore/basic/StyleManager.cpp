#include "basic/StyleManager.hpp"
#include "basic/Standard.hpp"
#include "utility/Utility.hpp"
#include "../libstyle/OctoStyle.hpp"

#include <QProxyStyle>
#include <QFontDatabase>
#include <QWidget>
#include <QDebug>
#include <QStyleFactory>
#include <QtPlugin>

class OctoMYProxyStyle : public QProxyStyle{
		const qint64 OCTOMY_SLIDER_HANDLE_SIZE=35;
		const qint64 OCTOMY_CURSOR_WSIZE=3;
	public:
		int pixelMetric ( PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const{
			OC_METHODGATE();
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
	OC_METHODGATE();
}

void StyleManager::apply(){
	OC_METHODGATE();
	QCoreApplication *capp=static_cast<QCoreApplication *>(QCoreApplication::instance());
	QApplication *app=qobject_cast<QApplication *>(capp);
	//GUI enabled
	if (0!=app) {
		QWidget *w=app->activeWindow();
		if(0!=w){
			w->setUpdatesEnabled(false);
		}
		QIcon icon(":/icons/octomy_logo_bare.svg");
		app->setWindowIcon(icon);
		loadFonts(*app);
		applyStyle(*app);
		//applyStyleSheet(*app); //Disabled for now (it looks but ugly)
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
	OC_METHODGATE();
	const int fontId =	QFontDatabase::addApplicationFont ( name);
	if(fontId<0){
		qWarning() << "ERROR: Could not load font for "<<name<< "( "<<fontId<<" )";
		return false;
	}
	return true;
}




void StyleManager::loadFonts(QApplication &app){
	OC_METHODGATE();
	qputenv("QT_QPA_FONTDIR", "/");
	//Main
	/*
	loadFont (":/fonts/CenturyGothic/CenturyGothicStd.otf");
	loadFont (":/fonts/CenturyGothic/CenturyGothicStd-Bold.otf");
	loadFont (":/fonts/CenturyGothic/CenturyGothicStd-Italic.otf");
	loadFont (":/fonts/CenturyGothic/CenturyGothicStd-BoldItalic.otf");
	*/
	loadFont (":/fonts/Dosis/Dosis-Regular.otf");
	loadFont (":/fonts/Dosis/Dosis-Bold.otf");
	loadFont (":/fonts/Dosis/Dosis-ExtraBold.otf");
	loadFont (":/fonts/Dosis/Dosis-ExtraLight.otf");
	loadFont (":/fonts/Dosis/Dosis-Light.otf");
	loadFont (":/fonts/Dosis/Dosis-Medium.otf");
	loadFont (":/fonts/Dosis/Dosis-SemiBold.otf");
	//LCD
	loadFont (":/fonts/digital-7 (mono).ttf");
	//Set standard font
	QFont font;
	font.setFamily("Dosis");
	app.setFont(font);
}

void StyleManager::applyStyle(QApplication &app){
	OC_METHODGATE();
	/*
	QStringList availableStyles=QStyleFactory::keys();
	foreach (QString s, availableStyles) {
		qDebug()<<"STYLE: "<<s;
	}
	*/
	Q_INIT_RESOURCE(OctoStyle);
	app.setStyle(new OctoStyle);
	//app.setStyle(new OctoMYProxyStyle);
	app.setCursorFlashTime(500);
}


void StyleManager::applyStyleSheet(QApplication &app){
	OC_METHODGATE();
	app.setStyleSheet ( utility::fileToString(":/style/style.qss"));
}


