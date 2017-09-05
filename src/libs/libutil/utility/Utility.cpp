#include "utility/Utility.hpp"
#include "utility/Standard.hpp"

#include <QApplication>
#include <QButtonGroup>
#include <QComboBox>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QGraphicsEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QHeaderView>
#include <QHostAddress>
#include <QIcon>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QLayout>
#include <QMediaContent>
#include <QNetworkAddressEntry>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QStackedLayout>
#include <QString>
#include <QStringBuilder>
#include <QTableView>
#include <QTextStream>
#include <QToolTip>
#include <QUdpSocket>
#include <QWidget>
#include <QWindow>
#include <QSplitter>
#include <QImageWriter>
#include <QColor>


#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <errno.h>
#include <functional>
#include <iostream>
#include <locale>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

namespace utility
{



QString humanReadableElapsedSeconds(long double seconds,int prec,bool prep,long double resolution)
{
	struct stance {
		long double interval;
		QString name;
	};
	const stance stances[] {
		{0.0,"time"}
		,{0.000001,"ns"}
		,{0.001,"ms"}
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
	if(seconds<0.0) {
		seconds=-seconds;
		past=false;
	}
	if(std::isnan(seconds) ) {
		seconds=0.0;
	}
	if(0.0==seconds) {
		seconds=0.0;
		i=0;
	} else {
		const long double lim=qMax(seconds,resolution);
		while (i<l && lim > stances[i].interval) {
			i++;
		}
	}
	i=qMax(0,i-1);
	QString ret= (prep?(past?"":"in "):"");
	ret+=QString::number(seconds/ stances[i].interval,'f',prec);
	ret+=" ";
	ret+=stances[i].name;
	ret+=(prep?(past?" ago":""):"");
	return ret;
}


QString humanReadableElapsedMS(qint64 ms,int prec,bool prep,long double resolution)
{
	return humanReadableElapsedSeconds((long double)ms/1000.0,prec,prep,resolution);
}

QString formattedDateFromSeconds(long double seconds)
{
	qint64 ms=(qint64)(seconds*1000.0);
	return formattedDateFromMS(ms);
}

QString formattedDateFromMS(qint64 ms)
{
	return QDateTime::fromMSecsSinceEpoch(ms).toString("yyyy-MM-dd hh:mm:ss.zzz t") ;
}


QString humanReadableSize(qint64 bytes,int prec)
{
	struct stance {
		qint64 interval;
		QString name;
	};
	const qint64 K=1024;
	const stance stances[] {
		{1,"bytes"}
		,{K,"KiB"}
		,{K*K,"MiB"}
		,{K*K*K,"GiB"}
		,{K*K*K*K,"TiB"}
		,{K*K*K*K*K,"PiB"}
		,{K*K*K*K*K*K,"EiB"}
		,{K*K*K*K*K*K*K,"ZiB"}
		,{K*K*K*K*K*K*K*K,"YiB"}
	};
	int i=1,l=sizeof(stances)/sizeof(stance)-1;
	if(bytes<0) {
		bytes=-bytes;
	}
	if(std::isnan(bytes) ) {
		bytes=0;
	}
	if(0==bytes) {
		bytes=0;
		i=0;
	} else {
		while (i<l && bytes > stances[i].interval) {
			i++;
		}
	}
	i=qMax(0,i-1);
	const long double bytesf=bytes;
	const long double intervalf=stances[i].interval;
	QString ret=QString::number(bytesf/ intervalf,'f',prec);
	ret+=" ";
	ret+=stances[i].name;
	return ret;
}


//TODO: memory management
QLayout *createStraigtLayout(QWidget *w)
{
	QLayout *l=0;
	if( 0!=w) {
		l=w->layout();
		if(0==l ) {
			l=OC_NEW QGridLayout(w);
			w->setLayout(l);
		}
		if(0!=l) {
			makeFiller(w);
			l->setSizeConstraint(QLayout::SetDefaultConstraint);
			l->setContentsMargins(0,0,0,0);
		}
	}
	return l;
}

//TODO: memory management
QStackedLayout *createStackedLayout(QWidget *w)
{
	QStackedLayout *l=0;
	if( 0!=w) {
		l=OC_NEW QStackedLayout(w);
		w->setLayout(l);
		if(0!=l) {
			makeFiller(w);
			l->setSizeConstraint(QLayout::SetDefaultConstraint);
			l->setContentsMargins(0,0,0,0);
		}
	}
	return l;
}


void makeFiller(QWidget *w)
{
	if(0!=w) {
		QSizePolicy sp(QSizePolicy::Expanding,QSizePolicy::Expanding);
		sp.setHorizontalStretch(0);
		sp.setVerticalStretch(0);
		w->setSizePolicy(sp);
	}
}



int snprint_hex(char *dst, size_t size, const uint8_t *pbtData, const size_t szBytes)
{
	size_t  szPos;
	size_t res = 0;
	for (szPos = 0; szPos < szBytes; szPos++) {
		res += snprintf(dst + res, size - res, "%02X", pbtData[szPos]);
	}
	res += sizeof('\0');//snprintf(dst + res, size - res, "\0");
	return res;
}


QString toHEX(const uint8_t *pbtData, const size_t szBytes)
{
	QString out;
	size_t  szPos;
	//		size_t res = 0;
	uint8_t dst[20];
	for (szPos = 0; szPos < szBytes; szPos++) {
		snprintf((char *)dst, 0, "%02X", pbtData[szPos]);
		out+=QString::fromLatin1((char *)dst,2);
	}
	return out;
}



QString  hexdump(void *mem, unsigned int len, QString topic)
{
	static const int HEXDUMP_COLS=16;
	QStringList sl;
	if(!topic.trimmed().isEmpty()) {
		sl<<"----- "<<topic<<" -----\n";
	}
	for(unsigned int i = 0; i < len + ((len % HEXDUMP_COLS) ? (HEXDUMP_COLS - len % HEXDUMP_COLS) : 0); i++) {
		//print offset
		if(i % HEXDUMP_COLS == 0) {
			sl<<QString("0x%1: ").arg(i,4,16,(QChar)QLatin1Char('0'));
		}
		//print hex data
		if(i < len) {
			sl<<QString("%1").arg((int)(0xFF & ((char*)mem)[i]),2,16,(QChar)QLatin1Char('0'));
		} else {
			//end of block, just aligning for ASCII dump
			sl<<"   ";
		}
		//print ASCII dump
		if(i % HEXDUMP_COLS == (HEXDUMP_COLS - 1)) {
			sl<<"  ";
			for(unsigned int j = i - (HEXDUMP_COLS - 1); j <= i; j++) {
				if(j >= len) {
					//end of block, not really printing
					sl<<" ";
				} else if(isprint((((char*)mem)[j] & 0x7F))) {
					//printable char
					sl<<QString((QChar)(0xFF & ((char*)mem)[j]));
				} else {
					//other char
					sl<<".";
				}
			}
			sl<<"\n";
		}
	}
	return sl.join("");
}


//TODO: memory management
void addIconToLayout(QString name,QLayout &l,int w,int h)
{
	if(name!="") {
		QPixmap pm(name);
		QIcon ic(pm);
		QPixmap pm2 = ic.pixmap(w>0?w:100,h>0?h:100);
		QLabel *lb=OC_NEW QLabel();
		lb->setPixmap(pm2);
		lb->setAlignment(Qt::AlignCenter);
		l.addWidget(lb);
	}
	// l->addWidget(&but);
}


void clearWidget(QWidget *par)
{
	foreach (QWidget * w, par->findChildren<QWidget*>()) {
		if (! w->windowFlags() & Qt::Window) {
			delete w;
			w=nullptr;
		}
	}
}

void clearLayout(QLayout* layout, bool deleteWidgets)
{
	while (QLayoutItem* item = layout->takeAt(0)) {
		if (deleteWidgets) {
			if (QWidget* widget = item->widget()) {
				delete widget;
			}
		} else if (QLayout* childLayout = item->layout()) {
			clearLayout(childLayout, deleteWidgets);
		}
		delete item;
		item=nullptr;
	}
}


bool widgetIsInLayout(const QLayout* layout, const QWidget* _someWidget )
{
	for(int i = 0; i < layout->count(); i++) {
		QLayoutItem* item = layout->itemAt(i);
		if(item->widget() == _someWidget ) {
			return true;
		}
		if(item->layout() && widgetIsInLayout(item->layout(), _someWidget)  ) {
			return true;
		}
	}
	return false;
}

bool checkWidgetInsideLayout(const QWidget* w)
{
	if(0!=w) {
		QWidget * const p=(QWidget *)w->parent();
		if(0!=p) {
			QLayout * const l=p->layout();
			if(0!=l) {
				return widgetIsInLayout(l, w);
			}
		}
	}
	return false;
	/*
		return (0 != _someWidget) && 			(0!= _someWidget->parent() )  &&
				(0!=((QWidget *)_someWidget->parent())->layout())  &&
				widgetIsInLayout((QWidget *)_someWidget->parent())->layout(), _someWidget );*/
}

void setWidgetSize(QWidget *wg, int , int , int w, int h)
{
	if(0!=wg) {
		const bool ue=wg->updatesEnabled();
		wg->setUpdatesEnabled(false);
		wg->setMinimumSize(w,h);
		wg->setMaximumSize(w,h);
		wg->resize(w,h);
		wg->move(0,0);
		//wg->setGeometry(QRect(x,y,w,h));



		wg->setProperty("width",QVariant(w));
		wg->setProperty("height",QVariant(h));
		wg->setProperty("minWidth",QVariant(w));
		wg->setProperty("minHeight",QVariant(h));
		wg->setProperty("maxWidth",QVariant(w));
		wg->setProperty("maxHeight",QVariant(h));


		wg->setUpdatesEnabled(ue);
	}
}



void setWidgetSize(QWindow *wg, int , int , int w, int h)
{
	if(0!=wg) {
		wg->setMinimumSize(QSize(w,h));
		wg->setMaximumSize(QSize(w,h));
		wg->resize(w,h);
		wg->setPosition(0,0);
		//wg->setGeometry(QRect(x,y,w,h));
	}
}


QString humanReadablize(QString orig, int space, QString sep)
{
	QString out;
	for (int pos=0, l=orig.length(); pos<l; pos+=space) {
		out+=orig.midRef(pos,space);
		if(pos<l-space) {
			out+=sep;
		}
	}
	return out;
}


bool isPositiveInteger(QString s)
{
	QString p=s.replace(QRegExp("[^0-9]"),"");
	return p.length()==s.length();
}


void showToolTip(QWidget *widget,QString str)
{
	if(0!=widget) {
		QToolTip::showText( widget->mapToGlobal( QPoint( 0, 0 ) ), str );
	}
}





void showQuitButton()
{
	qDebug()<<"NEW BUTTON";
	QPushButton *quitButton=OC_NEW QPushButton("Q U I T");
	qDebug()<<"RECT";
	QRect rect (0,0,100,100);
	quitButton->setGeometry(rect);
	qDebug()<<"set style";
	quitButton->setStyleSheet("QPushButton{ background:red; color:white; font-weight:bold;}");
	qDebug()<<"set win flags";
	quitButton->setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint  |  Qt::WindowStaysOnTopHint);
	qDebug()<<"set attr";
	quitButton->setAttribute(Qt::WA_ShowWithoutActivating);
	qDebug()<<"conn";

	qDebug()<<"set show";
	quitButton->show();
	if(!QObject::connect(quitButton, SIGNAL(clicked()),quitButton,SLOT(quit()),OC_CONTYPE)) {
		delete quitButton;
		quitButton=nullptr;
		qWarning()<<"ERROR: Could not connect";

	}
	qDebug()<<"set quit on last";
	//app->setQuitOnLastWindowClosed(true);
	qDebug()<<"init but done";

}



QString fileToString(QString fn)
{
	QFile file(fn);
	if(file.open(QFile::ReadOnly)) {
		QTextStream in(&file);
		return in.readAll();
	}
	return "";
}


QByteArray fileToByteArray(QString fn)
{
	QFile file(fn);
	if(file.open(QFile::ReadOnly)) {
		return file.readAll();
	}
	return "";
}



bool ensureDirExistsForFile(QString fn)
{
	QFile file(fn);
	QDir dir = QFileInfo(file).absoluteDir();
	if(!dir.exists()) {
		return dir.mkpath(dir.absolutePath());
	}
	return true;
}


bool stringToFile(QString fn, QString data, bool append)
{
	QFile file(fn);
	if(file.open(append?(QFile::WriteOnly|QFile::Append):(QFile::WriteOnly))) {
		QTextStream out(&file);
		out<<data;
		out.flush();
		file.flush();
		file.close();
		sync();
		return true;
	}
	return false;
}


bool byteArrayToFile(QString fn, QByteArray data, bool append)
{
	QFile file(fn);
	if(file.open(append?(QFile::WriteOnly|QFile::Append):(QFile::WriteOnly))) {
		const qint64 total=data.size();
		qint64 acc=0;
		while(acc<total) {
			qint64 written=file.write(data.data(),total-acc);
			if(written<0) {
				return false;
			}
			//qDebug()<<"WRITE LOOP total="<<total<<", acc="<<acc<<", written="<<written;
			acc+=written;
		}
		file.flush();
		file.close();
		sync();
		return true;
	}
	return false;
}



qint64 parseDate(QString str,QString fmt)  //2013-08-14T20:26:01.7526+02:00
{
	QDateTime d = QDateTime::fromString(str,fmt);
	qint64 ts= d.toMSecsSinceEpoch();
	if(-210866803200000==ts) {
		ts=0;
	}
	//qDebug()<<"parseDate("<<str<<")="<<ts;
	return ts;
}


//TODO: add possibility to block on fake events
void sendFakeKeyEvent(QWidget *target, Qt::Key k, QString ks)
{
	if(0!=target) {
		QKeyEvent * press=OC_NEW QKeyEvent(QKeyEvent::KeyPress, (int )k,Qt::NoModifier,ks);
		QKeyEvent * release=OC_NEW QKeyEvent(QKeyEvent::KeyRelease, (int )k,Qt::NoModifier,ks);
		target->setFocus();
		//DELETE 2014-09-27
		QCoreApplication::postEvent ( target, press) ;
		//DELETE 2014-09-27
		QCoreApplication::postEvent ( target, release) ;
	}
}




void populateComboboxWithLocalAdresses(QComboBox &cb)
{
	cb.clear();
	QList<QNetworkInterface> list= QNetworkInterface::allInterfaces();
	for(int i=0; i<list.size(); i++) {
		QNetworkInterface inter = list.at(i);
		if (inter.flags().testFlag(QNetworkInterface::IsUp) && !inter.flags().testFlag(QNetworkInterface::IsLoopBack)) {
			QList<QNetworkAddressEntry> list2= inter.addressEntries();
			for(int j=0; j<list2.size(); j++) {
				QNetworkAddressEntry entry = list2.at(j);
				cb.addItem(entry.ip().toString());
			}
		}
	}
	cb.addItem("127.0.0.1");
}



QString perror()
{
	return QString::fromLatin1(strerror(errno));
}



bool verifyMedia(QString name)
{
	QString msg="";
	bool ret=false;
	if(""==name.trimmed()) {
		msg="content path empty";
	} else {
		QFileInfo file(name);
		for(int i=0; i<10 && file.isSymLink(); i++) {
			file=QFileInfo(file.symLinkTarget());
		}
		if(!file.exists()) {
			msg="media did not exist";
		} else if(!file.isFile()) {
			msg="media is not a regular file";
		} else if(!file.isReadable()) {
			msg="media not readable";
		} else {
			QMediaContent mc(name);
			if(mc.isNull()) {
				msg="media was null";
			} else {
				ret=true;
			}
		}
	}
	if(!ret) {
		qDebug()<<"VERIFY CONTENT FAILED: "<<msg;
	}
	return ret;
}


void clearButtonGroupSelection(QButtonGroup* group)
{
	if(0!=group) {
		QAbstractButton* checked = group->checkedButton();
		if (0!=checked) {
			bool ex=group->exclusive();
			group->setExclusive(false);
			checked->setChecked(false);
			group->setExclusive(ex);
		}
	} else {
		qWarning() << "ERROR: group was 0";
	}
}

bool hasSelectionButtonGroup(QButtonGroup* group)
{
	bool ret=(0!=group->checkedButton());
	//qDebug() <<group->objectName() <<(ret?"has selection ":"has no selection");
	return ret;
}


bool isEnabledButtonGroup(QButtonGroup* group)
{
	bool ret=false;
	QList<QAbstractButton*> buts=group->buttons();
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		ret=ret||b->isEnabled();
	}
	return ret;
}

void toggleButtonGroup(QButtonGroup* group,bool on,bool clearOnOff)
{
	QList<QAbstractButton*> buts=group->buttons();
	bool ex=group->exclusive();
	if(!on && clearOnOff) {
		group->setExclusive(false);
	}
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		b->setEnabled(on);
		if(!on && clearOnOff) {
			b->setChecked(false);
		}
	}
	group->setExclusive(ex);
}


void toggleButtonVisible(QButtonGroup* group,bool visible)
{
	QList<QAbstractButton*> buts=group->buttons();
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		b->setVisible(visible);
	}
}


QString getSelectedButtonName(QButtonGroup* group,QString def)
{
	if(0==group) {
		return def;
	}
	QAbstractButton *but=group->checkedButton();
	if(0==but) {
		return def;
	}
	return but->objectName();
}



qint32 getSelectedButtonIndex(QButtonGroup* group,qint32 def)
{
	if(0==group) {
		return def;
	}
	QAbstractButton *but=group->checkedButton();
	if(0==but) {
		return def;
	}
	qint32 index=0;
	QList<QAbstractButton*> buts=group->buttons();
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		if(b==but) {
			return index;
		}
		index++;
	}
	return def;
}



void fitContent(QTableView &tv)
{
	for (int i = 0, c=tv.horizontalHeader()->count(); i < c; ++i) {
		tv.horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
	}
}



void placeInScreen(QWidget &w,QPointF gravity, int pref)
{
	OC_FUNCTIONGATE();
	QDesktopWidget *desktop=QApplication::desktop();
	if(0!=desktop) {
		const int c=desktop->screenCount();
		pref=qMax(pref,0);
		pref=qMin(pref,c);
		const QRect geom=desktop->screenGeometry(pref);
		const QRect wgeom=w.geometry();
		QPointF gout((geom.width()-wgeom.width())*gravity.x()+geom.x(), (geom.height()-wgeom.height())*gravity.y()+geom.y());
		//qDebug()<<"GRAV="<<gravity<<", PREF="<<pref<<", WGEOM="<<wgeom<<", GEOM="<<geom<<", GOUT="<<gout;
		w.move(gout.toPoint());
	}
}

int getNonPrimaryScreen()
{
	OC_FUNCTIONGATE();
	QDesktopWidget *desktop=QApplication::desktop();
	int pref=0;
	if(0!=desktop) {
		const int c=desktop->screenCount();
		if(c>1) {
			const int ps=desktop->primaryScreen();
			if(ps==pref) {
				pref++;
			}
		}
	}
	return pref;
}

QString toHash(QString input, QCryptographicHash::Algorithm algo, bool upperCase)
{
	OC_FUNCTIONGATE();
	QCryptographicHash ch(algo);
	ch.addData(input.toUtf8());
	QByteArray raw=ch.result();
	QString out=raw.toHex();
	if(upperCase) {
		out=out.toUpper();
	} else {
		out=out.toLower();
	}
	return out;
}




void delay( int millisecondsToWait )
{
	QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
	while( QTime::currentTime() < dieTime ) {
		QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
	}
}

void toggleButtonSelection(QAbstractButton *button, bool on, bool clearOnOff)
{
	OC_FUNCTIONGATE();
	if(button->objectName() == "pushButtonRegister" && !on) {
		button->setVisible(false);
	}
	if(!on && clearOnOff) {
		button->setChecked(false);
	}
}

QString nameToNormalForm(QString name)
{
	OC_FUNCTIONGATE();
	if(name.isEmpty()) {
		return name;
	}
	name = name.toLower();
	name[0] = name[0].toUpper();
	for (int i = 1; i < name.length(); i++) {
		if(!name.at(i).isLetter() && name.at(i+1).isLetter()) {
			name[i+1] = name[i+1].toUpper();
		}
	}
	return name;
}

/*
void syncronizeEvents(){
	qint64 beginSync=QDateTime::currentMSecsSinceEpoch();
	QEventLoop *loop=OC_NEW QEventLoop (0);
	if(0!=loop){
		if(!connect(rep, SIGNAL(finished()), &loop, SLOT(quit()))){
			qWarning()<<"ERROR: Could not connect";
		}
		if(!connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()))){
			qWarning()<<"ERROR: Could not connect";
		}
		loop->exec();
		delete loop;
		loop=0;
	}
	qDebug() << "synchronization took "<<utility::humanReadableElapsedMS(QDateTime::currentMSecsSinceEpoch()-beginSync,2,false);
}
*/


QString networkErrorToString( QNetworkReply::NetworkError ne)
{
	OC_FUNCTIONGATE();
#define NETWORK_ERROR_TO_STRING_STANSA(a)  case(QNetworkReply::a): return #a + QStringLiteral("(") +QString::number(QNetworkReply::a) + QStringLiteral(")")
	switch(ne) {
		NETWORK_ERROR_TO_STRING_STANSA(NoError);
		NETWORK_ERROR_TO_STRING_STANSA(ConnectionRefusedError );
		NETWORK_ERROR_TO_STRING_STANSA(RemoteHostClosedError);
		NETWORK_ERROR_TO_STRING_STANSA(HostNotFoundError);
		NETWORK_ERROR_TO_STRING_STANSA(TimeoutError);
		NETWORK_ERROR_TO_STRING_STANSA(OperationCanceledError);
		NETWORK_ERROR_TO_STRING_STANSA(SslHandshakeFailedError);
		NETWORK_ERROR_TO_STRING_STANSA(TemporaryNetworkFailureError);
		NETWORK_ERROR_TO_STRING_STANSA(NetworkSessionFailedError);
		NETWORK_ERROR_TO_STRING_STANSA(BackgroundRequestNotAllowedError);
		NETWORK_ERROR_TO_STRING_STANSA(InsecureRedirectError);
		NETWORK_ERROR_TO_STRING_STANSA(TooManyRedirectsError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownNetworkError );
		NETWORK_ERROR_TO_STRING_STANSA(ProxyConnectionRefusedError );
		NETWORK_ERROR_TO_STRING_STANSA(ProxyConnectionClosedError);
		NETWORK_ERROR_TO_STRING_STANSA(ProxyNotFoundError);
		NETWORK_ERROR_TO_STRING_STANSA(ProxyTimeoutError);
		NETWORK_ERROR_TO_STRING_STANSA(ProxyAuthenticationRequiredError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownProxyError );
		NETWORK_ERROR_TO_STRING_STANSA(ContentAccessDenied );
		NETWORK_ERROR_TO_STRING_STANSA(ContentOperationNotPermittedError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentNotFoundError);
		NETWORK_ERROR_TO_STRING_STANSA(AuthenticationRequiredError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentReSendError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentConflictError);
		NETWORK_ERROR_TO_STRING_STANSA(ContentGoneError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownContentError);
		NETWORK_ERROR_TO_STRING_STANSA(ProtocolUnknownError);
		NETWORK_ERROR_TO_STRING_STANSA(ProtocolInvalidOperationError);
		NETWORK_ERROR_TO_STRING_STANSA(ProtocolFailure);
		NETWORK_ERROR_TO_STRING_STANSA(InternalServerError);
		NETWORK_ERROR_TO_STRING_STANSA(OperationNotImplementedError);
		NETWORK_ERROR_TO_STRING_STANSA(ServiceUnavailableError);
		NETWORK_ERROR_TO_STRING_STANSA(UnknownServerError);
	}
#undef NETWORK_ERROR_TO_STRING_STANSA
	return "UNKNOWN("+QString::number(ne)+")";
}



QString socketErrorToString( QAbstractSocket::SocketError se)
{
	OC_FUNCTIONGATE();
#define SOCKET_ERROR_TO_STRING_STANSA(a)  case(QAbstractSocket::a): return #a + QStringLiteral("(") +QString::number(QAbstractSocket::a) + QStringLiteral(")")
	switch(se) {
		SOCKET_ERROR_TO_STRING_STANSA(ConnectionRefusedError);
		SOCKET_ERROR_TO_STRING_STANSA(RemoteHostClosedError);
		SOCKET_ERROR_TO_STRING_STANSA(HostNotFoundError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketAccessError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketResourceError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketTimeoutError);
		SOCKET_ERROR_TO_STRING_STANSA(DatagramTooLargeError);
		SOCKET_ERROR_TO_STRING_STANSA(NetworkError);
		SOCKET_ERROR_TO_STRING_STANSA(AddressInUseError);
		SOCKET_ERROR_TO_STRING_STANSA(SocketAddressNotAvailableError);
		SOCKET_ERROR_TO_STRING_STANSA(UnsupportedSocketOperationError);
		SOCKET_ERROR_TO_STRING_STANSA(UnfinishedSocketOperationError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyAuthenticationRequiredError);
		SOCKET_ERROR_TO_STRING_STANSA(SslHandshakeFailedError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyConnectionRefusedError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyConnectionClosedError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyConnectionTimeoutError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyNotFoundError);
		SOCKET_ERROR_TO_STRING_STANSA(ProxyProtocolError);
		SOCKET_ERROR_TO_STRING_STANSA(OperationError);
		SOCKET_ERROR_TO_STRING_STANSA(SslInternalError);
		SOCKET_ERROR_TO_STRING_STANSA(SslInvalidUserDataError);
		SOCKET_ERROR_TO_STRING_STANSA(TemporaryError);
		SOCKET_ERROR_TO_STRING_STANSA(UnknownSocketError);
	}
#undef SOCKET_ERROR_TO_STRING_STANSA
	return "UNKNOWN("+QString::number(se)+")";
}






QString padstring(int level, QString base)
{
	QString out="";
	if(""==base) {
		base=" ";
	}
	const int l=base.length();
	for(int i=0; i<level; ++i) {
		out+=QString(base.at(i%l));
	}
	return out;
}



QImage tint(QImage src, QColor color, qreal strength)
{
	if(src.isNull()) {
		return QImage();
	}
	QGraphicsScene scene;
	QGraphicsPixmapItem item;
	item.setPixmap(QPixmap::fromImage(src));
	QGraphicsColorizeEffect effect;
	effect.setColor(color);
	effect.setStrength(strength);
	item.setGraphicsEffect(&effect);
	scene.addItem(&item);
	QImage res(src);
	QPainter ptr(&res);
	scene.render(&ptr, QRectF(), src.rect() );
	return res;
}

qreal moveSplitter(QSplitter &splitter, qreal pos)
{
	//qDebug()<<"MOVING SPLITTER: "<<&splitter << " TO "<<pos;
	QList<int> sz = splitter.sizes();
	qreal tot=sz[0]+sz[1];
	if(tot<=0.0){
		tot=1000.0f;
	}
	const qreal oldPos=(sz[0]+1.0f)/(tot+1.0f);
	//qDebug()<<" + SZ: "<<sz;	qDebug()<<" + TOT: "<<tot;	qDebug()<<" + OLDPOS: "<<oldPos;
	sz[0]=pos*tot;
	sz[1]=tot-sz[0];
	splitter.setSizes(sz);
	return oldPos;
}



QString toSoundex(QString in)
{
	QString out="";
	const int l=in.size();
	if(l<1) {
		return "0000";
	}
	in=in.toLower();
	out+=in[0];
	QChar lastChar='0';
	int ct=0;
	for(int i=1; i<l; ++i) {
		QCharRef ch=in[i];
		if(lastChar!=ch) {
			switch(ch.toLatin1()) {
			case('b'):
			case('f'):
			case('p'):
			case('v'): {
				out+='1';
				ct++;
			}
			break;
			case('c'):
			case('g'):
			case('j'):
			case('k'):
			case('q'):
			case('s'):
			case('x'):
			case('z'): {
				out+='2';
				ct++;
			}
			break;
			case('d'):
			case('t'): {
				out+='3';
				ct++;
			}
			break;
			case('l'): {
				out+='4';
				ct++;
			}
			break;
			case('m'):
			case('n'): {
				out+='4';
				ct++;
			}
			break;
			case('r'): {
				out+='5';
				ct++;
			}
			break;
			}
			if(ct==4) {
				break;
			}
			lastChar=ch;
		}
	}
	if(out.size()<4) {
		out+=QString("0").repeated(4-out.size());
	}
	return out;
}


int levenshtein_distance(const QString &s1, const QString  &s2)
{
	int s1len = s1.size();
	int s2len = s2.size();

	auto column_start = (decltype(s1len))1;

	auto column = OC_NEW decltype(s1len)[s1len + 1];
	std::iota(column + column_start, column + s1len + 1, column_start);

	for (auto x = column_start; x <= s2len; x++) {
		column[0] = x;
		auto last_diagonal = x - column_start;
		for (auto y = column_start; y <= s1len; y++) {
			auto old_diagonal = column[y];
			auto possibilities = {
				column[y] + 1,
				column[y - 1] + 1,
				last_diagonal + (s1[y - 1] == s2[x - 1]? 0 : 1)
			};
			column[y] = std::min(possibilities);
			last_diagonal = old_diagonal;
		}
	}
	auto result = column[s1len];
	delete[] column;
	column = nullptr;
	return result;
}




double fsec()
{
	quint64 now=QDateTime::currentMSecsSinceEpoch();
	double out=now;
	out/=1000.0;
	return out;
}


float frand()
{
	float v=qrand();
	v/=RAND_MAX;
	return v;
}







quint16	getFreeUDPPortForAddress(QHostAddress &adr)
{
	QUdpSocket udp;
	const quint16 port=0; //0 means let OS decide port for me
	if(udp.bind(adr, port)) {
		return udp.localPort();
	}
	return 0;
}



QList<QHostAddress> getAllLocalNetworkAddresses()
{
	QList<QHostAddress> out;
	QList<QNetworkInterface> allInterfaces = QNetworkInterface::allInterfaces();
	QNetworkInterface eth;

	foreach(eth, allInterfaces) {
		QList<QNetworkAddressEntry> allEntries = eth.addressEntries();
		QNetworkAddressEntry entry;
		foreach (entry, allEntries) {
			out << entry.ip();
			//qDebug() << entry.ip().toString() << "/" << entry.netmask().toString();
		}
	}
	return out;

}

QString localAddress()
{
	QList<QHostAddress> list=getAllLocalNetworkAddresses();
	for(QHostAddress adr:list) {
		if((QAbstractSocket::IPv4Protocol==adr.protocol()) && (!adr.isLoopback()) ) {
			return adr.toString();
		}
	}
	return "127.0.0.1";
}



static int ipow(int base, int exp)
{
	int result = 1;
	while (exp) {
		if (exp & 1) {
			result *= base;
		}
		exp >>= 1;
		base *= base;
	}

	return result;
}

QByteArray randomByteArray(int size)
{
	QByteArray out;
	int bpc=(ipow(2,sizeof(char)*8));
	for(int i=0; i<size; ++i) {
		const char ch=(qrand() % bpc);
		out.append(ch);
	}
	return out;
}

QRgb randomColor( )
{
	return QColor::fromHslF(frand(), frand(), frand(), frand() ).rgba();
}

QImage randomImage(int w, int h)
{
	QImage image( w,h, QImage::Format_ARGB32);
	image.fill(Qt::green);
	{
		QPainter painter(&image);
		painter.setPen(Qt::NoPen);
		const int rr=(qMin(w,h)/16);
		const int area=(w*h)/rr;

		for(int i=0; i<area; ++i) {
			const QColor c(qrand()%255, qrand()%255, qrand()%255,127);
			const int r=qrand()%rr;
			const int x=qrand()%w;
			const int y=qrand()%h;
			painter.setBrush(c);
			painter.drawEllipse(x-r,y-r,r*2,r*2);
		}
	}
	return image;
}

QByteArray imageToByteArray(QImage &image )
{
	const uchar *bits=image.bits();
	QByteArray ba((const char *)(bits), image.byteCount());
	return ba;
}

QByteArray randomImageByteArray(int w, int h)
{
	QImage image=randomImage(w,h);
	QByteArray ba=imageToByteArray(image);
	return ba;
}

QByteArray randomJPEGByteArray(int w, int h, int q)
{
	QImage image=randomImage(w,h);
	QBuffer bu;
	bu.open(QBuffer::WriteOnly);
	QImageWriter iw;
	iw.setFormat("jpeg");
	iw.setDevice(&bu);
	iw.setOptimizedWrite(true );
	iw.setQuality(q);
	iw.setCompression(9);
	iw.setProgressiveScanWrite(true );
	iw.write( image );
	bu.close();
	QByteArray ba=bu.buffer();
	//utility::byteArrayToFile("random_test.jpeg",ba );
	//qDebug()<<"Random JPEG image of size "<<w<<"x"<<h<<" was "<< utility::humanReadableSize(ba.size())<< "("<<(qreal)ba.size()/((qreal)w*h) << " bytes/pixel)";
	return ba;

}


}
