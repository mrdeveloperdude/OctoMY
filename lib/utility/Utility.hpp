#ifndef UTILITY_HPP
#define UTILITY_HPP

#include "basic/Standard.hpp"
#include <inttypes.h>
#include <stddef.h>
//#include <string> #include <vector>
#include <QString>
#include <QStringList>
#include <QAbstractButton>
#include <QNetworkReply>


class QLayout;
class QWidget;
class QString;
class QComboBox;
class QWindow;
class QStackedLayout;
class QButtonGroup;
class QTableView;

namespace utility{

	// UI

	QLayout *createStraigtLayout(QWidget *w);
	QStackedLayout *createStackedLayout(QWidget *w);
	void makeFiller(QWidget *w);
	void addIconToLayout(QString name,QLayout &l,int w=-1,int h=-1);
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
	QString  getSelectedButtonName(QButtonGroup* group,QString def="");
	void toggleButtonSelection(QAbstractButton* button, bool on, bool clearOnOff=true);
	void fitContent(QTableView &tv);
	void placeInScreen(QWidget &w,QPointF gravity=QPointF(0.5,0.5), int pref=-1);
	int getNonPrimaryScreen();
	void sendFakeKeyEvent(QWidget *target, Qt::Key k, QString ks);
	void populateComboboxWithLocalAdresses(QComboBox &cb);
	QImage tint(QImage src, QColor color, qreal strength=1.0);

	// String

	QString humanReadableElapsedSeconds(long double seconds,int prec=2, bool prep=true,long double resolution=0.000001);
	QString humanReadableElapsedMS(qint64 ms,int prec=2, bool prep=true,long double resolution=0.000001);
	QString formattedDateFromSeconds(long double seconds);
	QString formattedDateFromMS(qint64 ms);
	QString humanReadableSize(qint64 bytes,int prec=2);
	int snprint_hex(char *dst, size_t size, const uint8_t *pbtData, const size_t szBytes);
	QString toHEX(const uint8_t *pbtData, const size_t szBytes);
	QString  hexdump(void *mem, unsigned int len, QString topic="");
	QString humanReadablize(QString orig, int space=4, QString sep="-");
	bool isPositiveInteger(QString s);
	QString perror();
	qint64 parseDate(QString str,QString fmt="yyyy-MM-ddThh:mm:ss");
	QString toHash(QString input, QCryptographicHash::Algorithm algo=QCryptographicHash::Sha1, bool upperCase=true);
	QString nameToNormalForm(QString name);
	QString networkErrorToString( QNetworkReply::NetworkError ne);

	// File

	QString fileToString(QString fn);
	bool stringToFile(QString fn, QString data, bool append=false);
	bool verifyMedia(QString name);

	// Time

	void delay( int millisecondsToWait );
	//void syncronizeEvents();
}

#endif // UTILITY_HPP
