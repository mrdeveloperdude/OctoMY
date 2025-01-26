#include "Style.hpp"

#include <QDebug>
#include <QFontDatabase>




QFont loadFont(QString source, int size, bool doDebug){
	if(doDebug){
		qDebug()<<"Trying to load font " << source<<"with size"<<size;
	}
	const auto appFonts = QFontDatabase::addApplicationFont(source);
	if(appFonts>=0){
		if(doDebug){
			qDebug()<<"Found font";
		}
		return QFont(QFontDatabase::applicationFontFamilies(QFontDatabase::addApplicationFont(source)).at(0), size);
	}
	qWarning()<<"Could not find font";
	return QFont();
}



QColor mix(const QColor &a ,const QColor &b, qreal m){
	auto im=1.0-m;
	return QColor(
		a.red()* im + b.red()*m,
		a.green()*im + b.green()*m,
		a.blue()*im + b.blue()*m,
		a.alpha()*im + b.alpha()*m);
}
