#include "AudioUtils.hpp"

#include <QVector>
#include <QMap>
#include <QWidget>
#include <QPainter>
#include <QDebug>
#include <QProgressBar>
#include <QFile>
#include <QTextStream>
#include <QPainterPath>
#include <QList>
#include <QAudioDevice>
#include <QStringDecoder>
#include <QAudioDevice>
#include <QMediaDevices>

#include <cmath>


void FpsTimer::countFrame(){
	const auto now=QDateTime::currentMSecsSinceEpoch();
	mStatsFrames++;
	if(now-mLastStatsTime>5000){
		auto interval=now-mLastStatsTime;
		auto frameMs=interval/mStatsFrames;
		auto fps=(mStatsFrames*1000)/interval;
		mLastStatsTime=now;
		mStatsFrames=0;
		qDebug()<<"FPS:"<<fps<<", frame ms"<<frameMs;
	}
}



QString syncProgressToString(int total, int partial, int parsed, int failed){
	QString out;
	if(partial<total){
		static QString fmt(" Partial %1 (%2%)");
		auto pct=((partial+1)*100)/(total+1);
		out+=fmt.arg(partial).arg(pct);
	}
	else if(parsed<total){
		static QString fmt(" Parsed %1 (%2%)");
		auto pct=((parsed+1)*100)/(total+1);
		out+=fmt.arg(parsed).arg(pct);
	}
	else{
		out=QString::number(total);
	}
	if(failed>0){
		out+=QString(" of %1 total (%2 failed)").arg(total).arg(failed);
	}
	return out.trimmed();
}

int levenshteinDistance(const QString &source, const QString &target)
{
	// Mostly stolen from https://qgis.org/api/2.14/qgsstringutils_8cpp_source.html
	
	if (source == target) {
		return 0;
	}
	
	const int sourceCount = source.count();
	const int targetCount = target.count();
	
	if (source.isEmpty()) {
		return targetCount;
	}
	
	if (target.isEmpty()) {
		return sourceCount;
	}
	
	if (sourceCount > targetCount) {
		return levenshteinDistance(target, source);
	}
	
	QVector<int> column;
	column.fill(0, targetCount + 1);
	QVector<int> previousColumn;
	previousColumn.reserve(targetCount + 1);
	for (int i = 0; i < targetCount + 1; i++) {
		previousColumn.append(i);
	}
	
	for (int i = 0; i < sourceCount; i++) {
		column[0] = i + 1;
		for (int j = 0; j < targetCount; j++) {
			column[j + 1] = std::min({
										 1 + column.at(j),
										 1 + previousColumn.at(1 + j),
										 previousColumn.at(j) + ((source.at(i) == target.at(j)) ? 0 : 1)
									 });
		}
		column.swap(previousColumn);
	}
	
	return previousColumn.at(targetCount);
}


QString longestCommonSubsequence(const QString &source, const QString &target)
{
	// Mostly stolen from https://www.geeksforgeeks.org/printing-longest-common-subsequence/
	
	QMap<int, QMap<int, int> > l;
	for (int i = 0; i <= source.count(); i++) {
		for (int j = 0; j <= target.count(); j++) {
			if (i == 0 || j == 0) {
				l[i][j] = 0;
			} else if (source.at(i - 1) == target.at(j - 1)) {
				l[i][j] = l[i - 1][j - 1] + 1;
			} else {
				l[i][j] = std::max(l[i - 1][j], l[i][j - 1]);
			}
		}
	}
	
	int i = source.count();
	int j = target.count();
	int index = l[source.count()][target.count()];
	QString longestCommonSubsequence;
	while (i > 0 && j > 0) {
		if (source.at(i - 1) == target.at(j - 1)) {
			longestCommonSubsequence[index - 1] = source.at(i - 1);
			i--;
			j--;
			index--;
		} else if (l[i - 1][j] > l[i][j - 1]) {
			i--;
		} else {
			j--;
		}
	}
	
	return longestCommonSubsequence;
}


QString soundex( const QString &string )
{
	if ( string.isEmpty() )
		return QString();
	
	QString tmp = string.toUpper();
	
	//strip non character codes, and vowel like characters after the first character
	QChar *char1 = tmp.data();
	QChar *char2 = tmp.data();
	int outLen = 0;
	for ( int i = 0; i < tmp.length(); ++i, ++char2 )
	{
		if ( ( *char2 ).unicode() >= 0x41 && ( *char2 ).unicode() <= 0x5A && ( i == 0 || ( ( *char2 ).unicode() != 0x41 && ( *char2 ).unicode() != 0x45
																						   && ( *char2 ).unicode() != 0x48 && ( *char2 ).unicode() != 0x49
																						   && ( *char2 ).unicode() != 0x4F && ( *char2 ).unicode() != 0x55
																						   && ( *char2 ).unicode() != 0x57 && ( *char2 ).unicode() != 0x59 ) ) )
		{
			*char1 = *char2;
			char1++;
			outLen++;
		}
	}
	tmp.truncate( outLen );
	
	QChar *tmpChar = tmp.data();
	tmpChar++;
	for ( int i = 1; i < tmp.length(); ++i, ++tmpChar )
	{
		switch ( ( *tmpChar ).unicode() )
		{
		case 0x42:
		case 0x46:
		case 0x50:
		case 0x56:
			tmp.replace( i, 1, QChar( 0x31 ) );
			break;
			
		case 0x43:
		case 0x47:
		case 0x4A:
		case 0x4B:
		case 0x51:
		case 0x53:
		case 0x58:
		case 0x5A:
			tmp.replace( i, 1, QChar( 0x32 ) );
			break;
			
		case 0x44:
		case 0x54:
			tmp.replace( i, 1, QChar( 0x33 ) );
			break;
			
		case 0x4C:
			tmp.replace( i, 1, QChar( 0x34 ) );
			break;
			
		case 0x4D:
		case 0x4E:
			tmp.replace( i, 1, QChar( 0x35 ) );
			break;
			
		case 0x52:
			tmp.replace( i, 1, QChar( 0x36 ) );
			break;
		}
	}
	
	//remove adjacent duplicates
	char1 = tmp.data();
	char2 = tmp.data();
	char2++;
	outLen = 1;
	for ( int i = 1; i < tmp.length(); ++i, ++char2 )
	{
		if ( *char2 != *char1 )
		{
			char1++;
			*char1 = *char2;
			outLen++;
			if ( outLen == 4 )
				break;
		}
	}
	tmp.truncate( outLen );
	if ( tmp.length() < 4 )
	{
		tmp.append( "000" );
		tmp.truncate( 4 );
	}
	
	return tmp;
}


float aspect(QRect &r){
	return (r.width()+1)/(r.height()+1);
}


int floorint(float f){
	return static_cast<int>(floor(f));
}



QPalette generateNoticePalette(){
	QPalette palette;
	QBrush brush(QColor(0, 0, 0, 255));
	QBrush brush1(QColor(226, 172, 10, 255));
	QBrush brush2(QColor(255, 215, 95, 255));
	QBrush brush3(QColor(240, 193, 52, 255));
	QBrush brush4(QColor(113, 86, 5, 255));
	QBrush brush5(QColor(151, 115, 6, 255));
	QBrush brush6(QColor(255, 255, 255, 255));
	QBrush brush7(QColor(240, 213, 132, 255));
	QBrush brush8(QColor(255, 255, 220, 255));
	palette.setBrush(QPalette::AlternateBase, brush7);
	palette.setBrush(QPalette::Base, brush6);
	palette.setBrush(QPalette::BrightText, brush6);
	palette.setBrush(QPalette::Button, brush1);
	palette.setBrush(QPalette::ButtonText, brush);
	palette.setBrush(QPalette::Dark, brush4);
	palette.setBrush(QPalette::Light, brush2);
	palette.setBrush(QPalette::Mid, brush5);
	palette.setBrush(QPalette::Midlight, brush3);
	palette.setBrush(QPalette::Shadow, brush);
	palette.setBrush(QPalette::Text, brush);
	palette.setBrush(QPalette::ToolTipBase, brush8);
	palette.setBrush(QPalette::ToolTipText, brush);
	palette.setBrush(QPalette::Window, brush1);
	palette.setBrush(QPalette::WindowText, brush);
	return palette;
}



void colorBG(QWidget *w, QColor c){
	if(nullptr!=w){
		QPalette palette;
		palette.setColor(QPalette::Window, c);
		w->setPalette(palette);
		w->setAutoFillBackground(true);
	}
}

void transparentize(QWidget *w){
	if(nullptr!=w){
		w->setAttribute(Qt::WA_NoSystemBackground);
		w->setAttribute(Qt::WA_TranslucentBackground);
		w->setAttribute(Qt::WA_TransparentForMouseEvents);
		w->setWindowFlags(Qt::FramelessWindowHint);
		w->setStyleSheet("background-color: rgba(0,0,0,0)");
	}
}


void drawRoundedBar(QPainter &painter, QRectF rect, qreal rx, qreal ry){
	QPainterPath path;
	path.addRoundedRect(rect, rx, ry, Qt::AbsoluteSize);
	painter.drawPath(path);
}



void antialiasHints(QPainter &painter){
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.setRenderHint(QPainter::SmoothPixmapTransform , true);
	painter.setRenderHint(QPainter::TextAntialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing , true);
}

QString capitalize(const QString &str)
{
	if(str.size()<=0){
		return "";
	}
	QString tmp = str;
	tmp = tmp.toLower();
	tmp[0] = str[0].toUpper();
	return tmp;
}



void drawPlaceholder(QPainter &painter, QRectF rect){
	painter.drawRect(rect);
	painter.drawLine(rect.x(),rect.y(),rect.x()+rect.width(),rect.y()+rect.height());
	painter.drawLine(rect.x(), rect.y()+rect.height() ,rect.x()+rect.width(), rect.y());
	painter.drawLine(rect.center(), QPointF(10,10));
}


void ghostHide(QWidget *widget){
	if(nullptr!=widget){
		QSizePolicy sp_retain = widget->sizePolicy();
		sp_retain.setRetainSizeWhenHidden(true);
		widget->setSizePolicy(sp_retain);
	}
}

void colorProgressBar(QProgressBar *pb, QColor c){
	static QString style("QProgressBar::chunk {background: %1;}");
	if(nullptr!=pb){
		QPalette p;
		p.setColor(QPalette::Highlight, c);
		pb->setPalette(p);
		pb->setStyleSheet(style.arg(c.name()));
	}
}

QColor mix(QColor a, QColor b, float t){
	t=qBound(0.0,t,1.0);
	const float ti=1.0-t;
	QColor ret;
	ret.setRgbF(
				(a.redF()   * t + b.redF()   * ti),
				(a.greenF() * t + b.greenF() * ti),
				(a.blueF()  * t + b.blueF()  * ti),
				(a.alphaF()*t+b.alphaF()*ti));
	//qDebug()<<"a"<<a<<"b"<<b<<"ret"<<ret;
	return ret;
}


QString decodePossibleUTF8String(QByteArray ba){
	auto toUtf8 = QStringDecoder(QStringDecoder::Utf8);
	QString text = toUtf8(ba);
	if(toUtf8.hasError()){
		auto toLatin = QStringDecoder(QStringDecoder::Latin1);
		QString text = toLatin(ba);
	}
	return text;
}



QStringDecoder guessCodecFromByteArray(QByteArray ba){
	auto toUtf8 = QStringDecoder(QStringDecoder::Utf8);
	QString text = toUtf8(ba);
	if(!toUtf8.hasError()){
		return toUtf8;
	}
	return QStringDecoder(QStringDecoder::Latin1);
}


QByteArray fileToBytearray(QString fn){
	QFile f(fn);
	if (!f.open(QFile::ReadOnly )) return QByteArray();
	return f.readAll();
}


QString fileToString(QString fn){
	/*
	QFile f(fn);
	if (!f.open(QFile::ReadOnly | QFile::Text)) return "";
	QTextStream in(&f);
	return in.readAll();
	*/
	return decodePossibleUTF8String(fileToBytearray(fn));
}


QPixmap loadPixmap(QString fn){
	QPixmap pm;
	pm.load(fn);
	return pm;
}



QString humanReadableElapsedSeconds(long double seconds,int prec,bool prep,long double resolution)
{
	struct stance {
		long double interval;
		QString name;
		quint8 pad[8]= {0}; // Get rid of alignment padding warning
	};
	const stance stances[] {
		{0.0l,"time"}
		,{0.000001l,"ns"}
		,{0.001l,"ms"}
		,{1,"sec"}
		,{60,"min"}
		,{60*60,"hr"}
		,{60*60*24,"days"}
		,{60*60*24*7,"weeks"}
		,{60*60*24*30,"months"}
		,{60*60*24*365,"years"}
		,{60*60*24*365*10,"decades"}
		//		,{60*60*24*365*100,"centuries"}
		//		,{60*60*24*365*1000,"millenia"}
	};
	int i=1,l=sizeof(stances)/sizeof(stance)-1;
	bool past=true;
	if(seconds<0.0l) {
		seconds=-seconds;
		past=false;
	}
	if(std::isnan(seconds) ) {
		seconds=0.0l;
	}
	if(0.0l==seconds) {
		seconds=0.0l;
		i=0;
	} else {
		const long double lim=qMax(seconds,resolution);
		while (i<l && lim > stances[i].interval) {
			i++;
		}
	}
	i=qMax(0,i-1);
	QString ret= (prep?(past?"":"in "):"");
	ret+=QString::number(static_cast<double>(seconds / stances[i].interval), 'f', prec);
	ret+=" ";
	ret+=stances[i].name;
	ret+=(prep?(past?" ago":""):"");
	return ret;
}

void drawOutlinedText(QPainter &painter, QRectF &rect, QString &text, float alphaF, qreal outlineWidth, QColor fillColor, QColor outlineColor){
	const int alpha=floor(alphaF*255.0f);
	QTextOption opt(Qt::AlignCenter);
	fillColor.setAlpha(alpha);
	painter.setBrush(fillColor);
	outlineColor.setAlpha(alpha);
	painter.setPen(QPen(outlineColor, outlineWidth));
	QPainterPath path;
	path.addText(0,0,painter.font(),text);
	auto bounds=path.boundingRect();
	auto center=rect.center()-bounds.center();
	painter.save();
	painter.translate(center);
	painter.drawPath(path);
	painter.restore();
}


CropPix cropImage(QImage image, bool doDebug){
	CropPix cp;
	cp.rect=image.rect();
	if(false){
		auto const w=cp.rect.width();
		auto const h=cp.rect.height();
		// Top
		for(int y=0;y<h;++y){
			for(int x=0;x<w;++x){
				const QRgb p=image.pixel(x,y);
				if(qAlpha(p)>0){
					cp.rect.setBottom(y);
					break;
				}
			}
		}
		// Bottom
		for(int y=h-1;y>0;--y){
			for(int x=0;x<w;++x){
				const QRgb p=image.pixel(x,y);
				if(qAlpha(p)>0){
					cp.rect.setTop(y);
					break;
				}
			}
		}
		// Left
		for(int x=0;x<w;++x){
			for(int y=0;y<h;++y){
				const QRgb p=image.pixel(x,y);
				if(qAlpha(p)>0){
					cp.rect.setRight(x);
					break;
				}
			}
		}
		// Right
		for(int x=w-1;x>0;--x){
			for(int y=0;y<h;++y){
				const QRgb p=image.pixel(x,y);
				if(qAlpha(p)>0){
					cp.rect.setLeft(x);
					break;
				}
			}
		}
		//qDebug()<<"Crop rect:" <<cp.rect<<cp.rect.top()<<cp.rect.bottom()<<cp.rect.left()<<cp.rect.right();
		auto cropped=image.copy(cp.rect);
		if(doDebug){
			QPainter p(&cropped);
			p.setPen(Qt::blue);
			int b=0;
			p.drawRect(b, b, cropped.width()-b*2-1, cropped.height()-b*2-1);
		}
		
		cp.pixmap=QPixmap::fromImage(cropped);
	}else{
		cp.pixmap=QPixmap::fromImage(image);
	}
	return cp;
}


void drawFancyString(QPainter &painter, QRectF rect, QString str, int size, QColor fg, QColor bg, int fontFlags){
	auto font =painter.font();
	auto ss=(size*1)/15;
	font.setPixelSize(size);
	painter.setFont(font);
	painter.setPen(bg);
	painter.drawText(rect, fontFlags, str);
	rect.translate(-ss, -ss);
	painter.setPen(fg);
	painter.drawText(rect, fontFlags, str);
	rect.translate(ss, ss);
}


const QString msToMinsec(qint64 ms){
	const static qint64 MS_IN_MIN=(1000*60);
	auto min=ms/MS_IN_MIN;
	ms-=(min*MS_IN_MIN);
	auto sec=ms/1000;
	return QString("%1:%2").arg(min).arg(sec, 2, 10, QLatin1Char('0'));
}

void giantCross(QPainter &painter, QPointF p, qreal sz){
	painter.setPen(Qt::white);
	painter.drawLine(p.x(), p.y(), p.x()+sz, p.y());
	painter.drawLine(p.x(), p.y(), p.x()-sz, p.y());
	painter.drawLine(p.x(), p.y(), p.x(), p.y()+sz);
	painter.drawLine(p.x(), p.y(), p.x(), p.y()-sz);
}

void consoleFloat(float f){
	QString s;
	int m=100;
	float fc=f*0.5+0.5;
	for(int i=0;i<m;++i){
		s+=(i<m*fc)?"#":" ";
	}
	qDebug().nospace().noquote()<<(f<-1?"<":"|")<<s<<(f>1?"> ":"| ")<<f;
}



QList<QAudioDevice> availableAudioDevices(QAudioFormat format, bool output){
	QList<QAudioDevice> out;
	QString deviceType(output?"output":"input");
	const auto allDevices = output?QMediaDevices::audioOutputs():QMediaDevices::audioInputs();
	int skipped=0;
	for (const QAudioDevice &device : allDevices){
		auto name=device.description();
		if(device.isFormatSupported(format)){
			qDebug() << " + " << device.description()<< "("<<deviceType<<") found";
			out << device;
		}
		else{
			qWarning()<<name<<"did not support format"<<format;
			skipped++;
		}
	}
	qDebug()<<"Skipped"<<skipped<<"unsuitable"<<deviceType<<"devices";
	return out;
}




QString defaultAudioDeviceName(bool output){
	if(output){
		return QMediaDevices::defaultAudioOutput().description();
		
	}else{
		return QMediaDevices::defaultAudioInput().description();
	}
}


QAudioDevice audioDeviceForName(QString name, bool output){
	const auto allDevices = output?QMediaDevices::audioOutputs():QMediaDevices::audioInputs();
	QAudioDevice deviceInfo;
	for(auto &device: allDevices){
		if(device.description() == name){
			deviceInfo=device;
		}
	}
	return deviceInfo;
}

int sampleSizeForFormat(QAudioFormat f){
	switch(f.sampleFormat()){
	default:
	case(QAudioFormat::Unknown):return -1;
	case(QAudioFormat::UInt8):return 8;
	case(QAudioFormat::Int16):return 16;
	case(QAudioFormat::Int32): return 32;
	case(QAudioFormat::Float): return 32;
	}
	return -1;
}


QColor adjusted(QColor c, int r, int g, int b, int a){
	return QColor(c.red()+r, c.green()+g, c.blue()+b, c.alpha()+a);
}

QString sampleFormatsToString(QList<QAudioFormat::SampleFormat> formats){
	QString out;
	QSet set(formats.begin(), formats.end());
	if(set.contains(QAudioFormat::Unknown)){
		out+="?? ";
	}
	if(set.contains(QAudioFormat::UInt8)){
		out+="U8 ";
	}
	if(set.contains(QAudioFormat::Int16)){
		out+="I16 ";
	}
	if(set.contains(QAudioFormat::Int32)){
		out+="I32 ";
	}
	if(set.contains(QAudioFormat::Float)){
		out+="F32 ";
	}
	if(set.contains(QAudioFormat::NSampleFormats)){
		out+="NS ";
	}
	return out.trimmed();
}

QString channelConfigurationToString(QAudioFormat::ChannelConfig config){
	switch (config) {
	case QAudioFormat::ChannelConfigMono:{
		return "Mono";
	}break;
	case QAudioFormat::ChannelConfigStereo:{
		return "Stereo";
	}break;
	case QAudioFormat::ChannelConfig2Dot1:{
		return "Surround 2.1";
	}break;
	case QAudioFormat::ChannelConfig3Dot0:{
		return "Surround 3.0";
	}break;
	case QAudioFormat::ChannelConfig3Dot1:{
		return "Surround 3.1";
	}break;
	case QAudioFormat::ChannelConfigSurround5Dot0:{
		return "Surround 5.0";
	}break;
	case QAudioFormat::ChannelConfigSurround5Dot1:{
		return "Surround 5.1";
	}break;
	case QAudioFormat::ChannelConfigSurround7Dot0:{
		return "Surround 7.0";
	}break;
	case QAudioFormat::ChannelConfigSurround7Dot1:{
		return "Surround 7.1";
	}break;
	default:{
			return "Unknown";
	}break;
	}
}


