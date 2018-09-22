#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "Standard.hpp"
#include <inttypes.h>
#include <stddef.h>
//#include <string> #include <vector>
#include <QString>
#include <QStringList>
#include <QWidget>
#include <QAbstractButton>
#include <QtNetwork>


class QLayout;
class QWidget;
class QString;
class QComboBox;
class QWindow;
class QStackedLayout;
class QButtonGroup;
class QTableView;
class QHostAddress;
class QSplitter;
class QStackedWidget;

namespace utility
{

// UI

QLayout *createStraigtLayout(QWidget *w);
QStackedLayout *createStackedLayout(QWidget *w);
void makeFiller(QWidget *w);
void addIconToLayout(QString name,QLayout &l,int w=-1,int h=-1);
void clearStackedWidget(QStackedWidget *stackedWidget, bool deleteWidgets=false);
void clearWidget(QWidget *par);
void clearLayout(QLayout* layout, bool deleteWidgets = true);
bool checkWidgetInsideLayout(const QWidget* _someWidget);
bool widgetIsInLayout(const QLayout* layout, const QWidget* _someWidget );
void setWidgetSize(QWidget *wg, int x, int y, int w, int h);
void setWidgetSize(QWindow *wg, int x, int y, int w, int h);
void showToolTip(QWidget *widget,QString str);
void clearButtonGroupSelection(QButtonGroup* group);
bool hasSelectionButtonGroup(QButtonGroup* group);
bool isEnabledButtonGroup(QButtonGroup* group);
void toggleButtonGroup(QButtonGroup* group,bool on,bool clearOnOff=true);
void toggleButtonVisible(QButtonGroup* group,bool visible);
QString selectedButtonName(QButtonGroup* group,QString def="");
qint32 selectedButtonIndex(QButtonGroup* group,qint32 def);
void setSelectedButtonIndex(QButtonGroup* group, const qint32 index);
void toggleButtonSelection(QAbstractButton* button, bool on, bool clearOnOff=true);
void fitContent(QTableView &tv);
void placeInScreen(QWidget &w,QPointF gravity=QPointF(0.5,0.5), int pref=-1);
int nonPrimaryScreen();
void sendFakeKeyEvent(QWidget *target, Qt::Key k, QString ks);
void populateComboboxWithLocalAdresses(QComboBox &cb);
QImage tint(QImage src, QColor color, qreal strength=1.0);
qreal moveSplitter(QSplitter &splitter, qreal pos);
void drawText(QPainter & painter, qreal x, qreal y, Qt::Alignment flags, const QString & text, QRectF * boundingRect = nullptr);
void drawText(QPainter & painter, const QPointF & point, Qt::Alignment flags, const QString & text, QRectF * boundingRect = {});

template <typename T>
void pack(T *stacked);

// String

QString humanReadableElapsedSeconds(long double seconds,int prec=2, bool prep=true,long double resolution=0.000001);
QString humanReadableElapsedMS(qint64 ms,int prec=2, bool prep=true,long double resolution=0.000001);
QString formattedDateFromSeconds(long double seconds);
QString formattedDateFromMS(qint64 ms);
QString humanReadableSize(qint64 bytes,int prec=2);
size_t snprint_hex(char *dst, size_t size, const uint8_t *pbtData, const size_t szBytes);
QString toHEX(const uint8_t *pbtData, const size_t szBytes);
QString hexdump(void *mem, unsigned int len, QString topic="");
QString humanReadablize(QString orig, int space=4, QString sep="-");
bool isPositiveInteger(QString s);
QString perror();
qint64 parseDate(QString str,QString fmt="yyyy-MM-ddThh:mm:ss");
QString toHash(QString input, QCryptographicHash::Algorithm algo=QCryptographicHash::Sha1, bool upperCase=true);
QString nameToNormalForm(QString name);
QString networkErrorToString( QNetworkReply::NetworkError ne);
QString socketErrorToString( QAbstractSocket::SocketError se);
QString padstring(int level, QString base=" ");
QString toSoundex(QString in);
int levenshtein_distance(const QString &s1, const QString  &s2);
QString handleCounterString(Qt::HANDLE h);
QString currentThreadID();

// File

QString fileToString(QString fn);
QByteArray fileToByteArray(QString fn);
QVariant jsonFileToVariant(QString fn);
bool ensureDirExistsForFile(QString fn);
bool variantToJsonFile(QString fn, QVariant data);
bool stringToFile(QString fn, QString data, bool append=false);
bool byteArrayToFile(QString fn, QByteArray data, bool append=false);
bool verifyMedia(QString name);

// Time

void delay( int millisecondsToWait );
//void syncronizeEvents();

template<typename T>
T currentMsecsSinceEpoch();

double fsec();
float frand();
double dfrand();


quint64 variantToMs(QVariant v);
QVariant msToVariant(quint64 ms);


// Networking
QList<QHostAddress> allLocalNetworkAddresses();
QString localAddress();
bool checkUDPAddress(QHostAddress address, quint16 port);
quint16	freeUDPPortForAddress(QHostAddress adr);
QHostAddress defaultGatewayAddress();

quint32 addressCloseness(QHostAddress a, QHostAddress b);
QHostAddress closestAddress(QList<QHostAddress> addresses, QHostAddress network);
bool isValidIPv4(const QString str);
bool isValidIPv6(const QString str);
bool isValidIPv4(const QHostAddress addr);
bool isValidIPv6(const QHostAddress addr);


// Random
QByteArray randomByteArray(int size);
QRgb randomColor( );
QImage randomImage(int w=512, int h=512 );
QByteArray imageToByteArray(QImage &image );
QByteArray randomImageByteArray(int w=512, int h=512 );
QByteArray randomJPEGByteArray(int w=512, int h=512, int q=85 );

//Data
bool mapIsIn(const QVariantMap &orig, const QVariantMap &in, const bool careAboutType=false);
void merge( QVariantMap  &c, QVariantMap  &b);





class HandleCounter
{
private:
	quint64 handleCounter;
	QMap<Qt::HANDLE, quint64> handleMap;
	QMutex handleMutex;

public:

	HandleCounter();
	QString handleCounterString(Qt::HANDLE h);

};




template<typename T>
T currentMsecsSinceEpoch()
{
	return static_cast<T>(QDateTime::currentMSecsSinceEpoch());
}



// Sync/thread/eventloop/concurrency

// FROM https://stackoverflow.com/questions/21646467/how-to-execute-a-functor-or-a-lambda-in-a-given-thread-in-qt-gcd-style/21653558#21653558
template <typename F>
struct FunctorEvent : public QEvent {
	using Functor = typename std::decay<F>::type;
	Functor functor;
	explicit FunctorEvent(Functor && fun) : QEvent(QEvent::None), functor(std::move(fun)) {}
	explicit FunctorEvent(const Functor & fun) : QEvent(QEvent::None), functor(fun) {}
	virtual ~FunctorEvent()
	{
		//qDebug()<<"CALLING FUNCTOR FOR FUNCTOR EVENT";
		functor();
		//qDebug()<<"DONE CALLING FUNCTOR FOR FUNCTOR EVENT";
	}
};

template <typename F>
static void postToObject(F && functor, QObject * obj = qApp)
{
	if (qobject_cast<QThread*>(obj)) {
		qWarning() << "posting a call to a thread object - consider using postToThread";
	}
	QCoreApplication::postEvent(obj, OC_NEW FunctorEvent<F>(std::forward<F>(functor)));
}

template <typename F>
static void postToThread(F && functor, QThread * thread = qApp->thread())
{
	QObject * obj = QAbstractEventDispatcher::instance(thread);
	Q_ASSERT(obj);
	QCoreApplication::postEvent(obj, OC_NEW FunctorEvent<F>(std::forward<F>(functor)));
}




class Q_CORE_EXPORT MutexTryLocker
{
private:
	Q_DISABLE_COPY(MutexTryLocker)
	quintptr val;
public:

	inline explicit MutexTryLocker(QBasicMutex *m) QT_MUTEX_LOCK_NOEXCEPT {
		Q_ASSERT_X((reinterpret_cast<quintptr>(m) & quintptr(1u)) == quintptr(0), "MutexTryLocker", "QMutex pointer is misaligned");
		val = quintptr(m);
		if (Q_LIKELY(m))
		{
			// call QMutex::lock() instead of QBasicMutex::lock()
			const bool is_locked=static_cast<QMutex *>(m)->try_lock();
			if(is_locked) {
				val |= 1;
			}
		}
	}
	inline ~MutexTryLocker()
	{
		unlock();
	}

	inline void unlock() Q_DECL_NOTHROW {
		if ((val & quintptr(1u)) == quintptr(1u))
		{
			val &= ~quintptr(1u);
			mutex()->unlock();
		}
	}

	inline void relock() QT_MUTEX_LOCK_NOEXCEPT {
		if (val)
		{
			if ((val & quintptr(1u)) == quintptr(0u)) {
				mutex()->lock();
				val |= quintptr(1u);
			}
		}
	}


	inline QMutex *mutex() const
	{
		return reinterpret_cast<QMutex *>(val & ~quintptr(1u));
	}



};


///////////////////////////////////////////


template <typename T>
void pack(T *stacked)
{
	stacked->updatesEnabled();
	stacked->setUpdatesEnabled(false);
	for(int i=0; i<stacked->count(); i++) {
		stacked->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	}
	QWidget *widget=stacked->currentWidget();
	widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	widget->resize(widget->minimumSizeHint());
	widget->adjustSize();
	stacked->resize(stacked->minimumSizeHint());
	stacked->adjustSize();
	stacked->setUpdatesEnabled(true);
}


}

#endif // UTILITY_HPP
