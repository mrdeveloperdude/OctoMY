#include "Ui.hpp"

#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QString>
#include <QRectF>
#include <QPoint>
#include <QImage>

#include <QDebug>
#include <QtMath>
#include <QList>

#include <QWindow>
#include <QWidget>
#include <QLayout>

#include <QStackedLayout>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QAbstractButton>

#include <QTableView>
#include <QHeaderView>
#include <QComboBox>

#include <QSplitter>
#include <QLabel>
#include <QToolTip>


#include <QKeyEvent>


#include <QApplication>
#include <QCoreApplication>

#include <QNetworkInterface>

namespace utility
{

namespace ui
{

// UI


//TODO: memory management
QLayout *createStraigtLayout(QWidget *w)
{
	OC_FUNCTIONGATE();
	QLayout *l= nullptr;
	if(nullptr != w) {
		l = w->layout();
		if(nullptr == l) {
			l = OC_NEW QGridLayout(w);
			w->setLayout(l);
		}
		if(nullptr != l) {
			makeFiller(w);
			l->setSizeConstraint(QLayout::SetDefaultConstraint);
			l->setContentsMargins(0, 0, 0, 0);
		}
	}
	return l;
}

//TODO: memory management
QStackedLayout *createStackedLayout(QWidget *w)
{
	OC_FUNCTIONGATE();
	QStackedLayout *l=nullptr;
	if(nullptr != w) {
		l=OC_NEW QStackedLayout(w);
		w->setLayout(l);
		if(nullptr != l) {
			makeFiller(w);
			l->setSizeConstraint(QLayout::SetDefaultConstraint);
			l->setContentsMargins(0, 0, 0, 0);
		}
	}
	return l;
}


void makeFiller(QWidget *w)
{
	OC_FUNCTIONGATE();
	if(nullptr != w) {
		QSizePolicy sp(QSizePolicy::Expanding,QSizePolicy::Expanding);
		sp.setHorizontalStretch(0);
		sp.setVerticalStretch(0);
		w->setSizePolicy(sp);
	}
}



//TODO: memory management
void addIconToLayout(QString name,QLayout &l,int w,int h)
{
	OC_FUNCTIONGATE();
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

void clearStackedWidget(QStackedWidget *stackedWidget, bool deleteWidgets)
{
	OC_FUNCTIONGATE();
	if(nullptr!=stackedWidget) {
		while(stackedWidget->count() > 0) {
			QWidget* widget = stackedWidget->widget(0);
			stackedWidget->removeWidget(widget);
			if(deleteWidgets) {
				widget->deleteLater();
			}
		}
	}
}

void clearWidget(QWidget *par)
{
	OC_FUNCTIONGATE();
	foreach (QWidget * w, par->findChildren<QWidget*>()) {
		if (! (w->windowFlags() & Qt::Window) ) {
			delete w;
			w=nullptr;
		}
	}
}

void clearLayout(QLayout* layout, bool deleteWidgets)
{
	OC_FUNCTIONGATE();
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


bool checkWidgetInsideLayout(const QWidget* w)
{
	OC_FUNCTIONGATE();
	if(nullptr!=w) {
		QWidget * const p=static_cast<QWidget *>(w->parent());
		if(nullptr!=p) {
			QLayout * const l=p->layout();
			if(nullptr!=l) {
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



bool widgetIsInLayout(const QLayout* layout, const QWidget* _someWidget )
{
	OC_FUNCTIONGATE();
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

void setWidgetSize(QWidget *wg, int, int, int w, int h)
{
	OC_FUNCTIONGATE();
	if(nullptr!=wg) {
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



void setWidgetSize(QWindow *wg, int, int, int w, int h)
{
	OC_FUNCTIONGATE();
	if(nullptr!=wg) {
		wg->setMinimumSize(QSize(w,h));
		wg->setMaximumSize(QSize(w,h));
		wg->resize(w,h);
		wg->setPosition(0,0);
		//wg->setGeometry(QRect(x,y,w,h));
	}
}


void showToolTip(QWidget *widget,QString str)
{
	OC_FUNCTIONGATE();
	if(nullptr!=widget) {
		QToolTip::showText( widget->mapToGlobal( QPoint( 0, 0 ) ), str );
	}
}



void clearButtonGroupSelection(QButtonGroup* group)
{
	OC_FUNCTIONGATE();
	if(nullptr!=group) {
		QAbstractButton* checked = group->checkedButton();
		if (nullptr!=checked) {
			bool ex=group->exclusive();
			group->setExclusive(false);
			checked->setChecked(false);
			group->setExclusive(ex);
		}
	} else {
		qWarning() << "ERROR: group was null";
	}
}

bool hasSelectionButtonGroup(QButtonGroup* group)
{
	OC_FUNCTIONGATE();
	bool ret=(nullptr!=group->checkedButton());
	//qDebug() <<group->objectName() <<(ret?"has selection ":"has no selection");
	return ret;
}


bool isEnabledButtonGroup(QButtonGroup* group)
{
	OC_FUNCTIONGATE();
	bool ret=false;
	QList<QAbstractButton*> buts=group->buttons();
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		ret=ret||b->isEnabled();
	}
	return ret;
}

void toggleButtonGroup(QButtonGroup* group, bool on, bool clearOnOff)
{
	OC_FUNCTIONGATE();
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


void toggleButtonVisible(QButtonGroup* group, bool visible)
{
	OC_FUNCTIONGATE();
	QList<QAbstractButton*> buts=group->buttons();
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		b->setVisible(visible);
	}
}


QString selectedButtonName(QButtonGroup* group, QString def)
{
	OC_FUNCTIONGATE();
	if(nullptr==group) {
		return def;
	}
	QAbstractButton *but=group->checkedButton();
	if(nullptr==but) {
		return def;
	}
	return but->objectName();
}



qint32 selectedButtonIndex(QButtonGroup* group, qint32 def)
{
	OC_FUNCTIONGATE();
	if(nullptr==group) {
		return def;
	}
	QAbstractButton *but=group->checkedButton();
	if(nullptr==but) {
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


void setSelectedButtonIndex(QButtonGroup* group, const qint32 index)
{
	OC_FUNCTIONGATE();
	if(nullptr==group) {
		return;
	}
	qint32 ct = 0;
	QList<QAbstractButton*> buts=group->buttons();
	for(QList<QAbstractButton*>::iterator it=buts.begin(),end=buts.end(); it!=end; ++it) {
		QAbstractButton *b=*it;
		if(nullptr != b && index == ct) {
			b->setChecked(true);
			return;
		}
		ct++;
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




void fitContent(QTableView &tv)
{
	for (int i = 0, c=tv.horizontalHeader()->count(); i < c; ++i) {
		tv.horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
	}
}



void placeInScreen(QWidget &w, QPointF gravity, int pref)
{
	OC_FUNCTIONGATE();
	//TODO: Update to support https://doc.qt.io/qt-6/qscreen.html operations instead
	/*
	QDesktopWidget *desktop=QApplication::desktop();
	if(nullptr!=desktop) {
		const int c=desktop->screenCount();
		pref=qMax(pref,0);
		pref=qMin(pref,c);
		const QRect geom=desktop->screenGeometry(pref);
		const QRect wgeom=w.geometry();
		QPointF gout((geom.width()-wgeom.width())*gravity.x()+geom.x(), (geom.height()-wgeom.height())*gravity.y()+geom.y());
		//qDebug()<<"GRAV="<<gravity<<", PREF="<<pref<<", WGEOM="<<wgeom<<", GEOM="<<geom<<", GOUT="<<gout;
		w.move(gout.toPoint());
	}
	*/
}

int nonPrimaryScreen()
{
	OC_FUNCTIONGATE();
	//TODO: Update to support https://doc.qt.io/qt-6/qscreen.html operations instead
	/*
	QDesktopWidget *desktop=QApplication::desktop();
	int pref=0;
	if(nullptr!=desktop) {
		const int c=desktop->screenCount();
		if(c>1) {
			const int ps=desktop->primaryScreen();
			if(ps==pref) {
				pref++;
			}
		}
	}
	return pref;
	*/
	return 0;
}



//TODO: add possibility to block on fake events
void sendFakeKeyEvent(QWidget *target, Qt::Key k, QString ks)
{
	OC_FUNCTIONGATE();
	if(nullptr!=target) {
		QKeyEvent * press=OC_NEW QKeyEvent(QKeyEvent::KeyPress, static_cast<int>(k),Qt::NoModifier,ks);
		QKeyEvent * release=OC_NEW QKeyEvent(QKeyEvent::KeyRelease, static_cast<int>(k),Qt::NoModifier,ks);
		target->setFocus();
		//DELETE 2014-09-27
		QCoreApplication::postEvent ( target, press) ;
		//DELETE 2014-09-27
		QCoreApplication::postEvent ( target, release) ;
	}
}




void populateComboboxWithLocalAdresses(QComboBox &cb)
{
	OC_FUNCTIONGATE();
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



qreal moveSplitter(QSplitter &splitter, qreal pos)
{
	OC_FUNCTIONGATE();
	//qDebug()<<"MOVING SPLITTER: "<<&splitter << " TO "<<pos;
	QList<int> sz = splitter.sizes();
	qreal tot=sz[0]+sz[1];
	if(tot<=0.0) {
		tot=1000.0l;
	}
	const qreal oldPos=(sz[0]+1.0)/(tot+1.0);
	//qDebug()<<" + SZ: "<<sz;	qDebug()<<" + TOT: "<<tot;	qDebug()<<" + OLDPOS: "<<oldPos;
	sz[0]=qFloor(pos*tot);
	sz[1]=qFloor(tot-sz[0]);
	splitter.setSizes(sz);
	return oldPos;
}




}
}

