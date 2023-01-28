#ifndef AUDIOUTILS_HPP
#define AUDIOUTILS_HPP

#include <QString>
#include <QRect>
#include <QPalette>
#include <QPainter>
#include <QPixmap>
#include <QDateTime>
#include <QAudioFormat>
#include <QStringDecoder>

#include <cmath>
#include <limits>

const auto dNaN=std::numeric_limits<double>::quiet_NaN();
const auto dInf = std::numeric_limits<double>::infinity();
const auto fNaN=std::numeric_limits<float>::quiet_NaN();
const auto fInf = std::numeric_limits<float>::infinity();


constexpr double TAU = 2.0 * M_PI;



class QProgressBar;
class QTextCodec;
class QAudioDevice;

#define CONNECTION_TYPE static_cast<Qt::ConnectionType>(Qt::QueuedConnection | Qt::UniqueConnection)



inline QAudioFormat getBaseAudioFormat(){
	QAudioFormat af;
	af.setSampleRate(44100);
	af.setChannelCount(1);
	af.setSampleFormat(QAudioFormat::Int16);
	return af;
}

static const auto baseAudioFormat=getBaseAudioFormat();


struct CropPix{
	QPixmap pixmap;
	QRect rect;
	void draw(QPainter &p, QPointF pos=QPointF()){
		p.drawPixmap(rect.topLeft()+pos, pixmap);
	}
	bool isNull() const{
		return pixmap.isNull();
	}
	void reset(){
		pixmap=QPixmap();
	}
};



struct FpsTimer{
	qint64 mLastStatsTime=QDateTime::currentMSecsSinceEpoch();
	int mStatsFrames=0;
	void countFrame();
};

void consoleFloat(float f);


QString syncProgressToString(int total, int partial, int parsed, int failed);

int levenshteinDistance(const QString &source, const QString &target);
QString longestCommonSubsequence(const QString &source, const QString &target);
QString soundex( const QString &string );

int floorint(float f);
float aspect(QRect &r);


inline float frand(){
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

inline float frand_s(){
	return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX))*2.0-1.0;
}

QPalette generateNoticePalette();

void colorBG(QWidget *w, QColor c=Qt::red);
void transparentize(QWidget *w);
void drawRoundedBar(QPainter &painter, QRectF rect, qreal rx=0, qreal ry=0);
void antialiasHints(QPainter &painter);

QString capitalize(const QString &str);
void drawPlaceholder(QPainter &painter, QRectF rect);
void ghostHide(QWidget *widget);
void colorProgressBar(QProgressBar *pb, QColor c);

QColor mix(QColor a,QColor b, float mix);

QStringDecoder guessCodecFromByteArray(QByteArray ba, bool output=false);

QString decodePossibleUTF8String(QByteArray ba);
QByteArray fileToBytearray(QString fn);
QString fileToString(QString fn);
QPixmap loadPixmap(QString fn);

QString humanReadableElapsedSeconds(long double seconds,int prec=2, bool prep=true,long double resolution=0.000001L);

template <typename T>
QString humanReadableElapsedMS(T ms,int prec=2,bool prep=true,long double resolution=0.000001L)
{
	return humanReadableElapsedSeconds(static_cast<long double>(ms) / 1000.0l, prec, prep, resolution);
}

void drawOutlinedText(QPainter &painter, QRectF &rect, QString &text, float alphaF=1.0, qreal outlineWidth=2.0, QColor fillColor=Qt::yellow, QColor outlineColor=Qt::black);
CropPix cropImage(QImage image, bool doDebug=false);

void drawFancyString(QPainter &painter, QRectF rect, QString str, int size, QColor fg, QColor bg, int fontFlags);
const QString msToMinsec(qint64 ms);

void giantCross(QPainter &painter, QPointF p, qreal sz=10000);

QList<QAudioDevice> availableAudioDevices(QAudioFormat format, bool output);

QString defaultAudioDeviceName(bool output);

QAudioDevice audioDeviceForName(QString name, bool output);

int sampleSizeForFormat(QAudioFormat f);

QColor adjusted(QColor c, int r=0, int g=0, int b=0, int a=0);

QString sampleFormatsToString(QList<QAudioFormat::SampleFormat> formats);
QString channelConfigurationToString(QAudioFormat::ChannelConfig config);




#endif // AUDIOUTILS_HPP
