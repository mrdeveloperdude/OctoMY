#include "PairingEditButtonDelegate.hpp"

#include "PairingWizard.hpp"
#include "Identicon.hpp"
#include "security/PortableID.hpp"


#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>



PairingEditButtonDelegate::PairingEditButtonDelegate(PairingWizard &pwiz)
	: QItemDelegate(&pwiz)
	, pwiz(pwiz)
{
}

PairingEditButtonDelegate::~PairingEditButtonDelegate()
{

}



QSizeF PairingEditButtonDelegate::calcSize(QSizeF ds, const qint32 w, const qint32 h, qreal const z)
{
	if(w<1 && h<1){
		return ds;
	}
	const qreal da=ds.width()/ds.height();
	//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
	QSizeF low(w,(qreal)h/da);
	QSizeF high((qreal)w*da,h);
	//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	const qreal zoom=(z<0)?0:(z>1)?1:z;
	const qreal izoom=1.0-zoom;
	return low*izoom+high*zoom;
}



QPixmap PairingEditButtonDelegate::svgToPixmap(QString fn, const  qint32 w, const qint32 h, const qreal zoom) const
{
	QSvgRenderer svg(fn);
	QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
	//qDebug()<<"Generating pixmap from identicon with size: "<<ds;
	QPixmap px(ds.toSize());
	px.fill(QColor(0,0,0,0));
	QPainter painter( &px );
	svg.render( &painter );
	//px.save("/tmp/px.png");
	return px;
}



static QSizeF calcSize(QSizeF ds,qint32 w,qint32 h,qreal zoom){
	if(w<1 && h<1){
		return ds;
	}
	const qreal da=ds.width()/ds.height();
	//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
	QSizeF low(w,(qreal)h/da);
	QSizeF high((qreal)w*da,h);
	//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	zoom=(zoom<0)?0:(zoom>1)?1:zoom;
	qreal izoom=1.0-zoom;
	return low*izoom+high*zoom;
}

static void drawSVG(QPainter *p, QString url, qint32 x,qint32 y, qint32 w,qint32 h, qreal zoom=0.0f){
	if(nullptr!=p){
		QSvgRenderer svg(url);
		QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
		QPixmap px(ds.toSize());
		px.fill(QColor(0,0,0,0));
		QPainter painter( &px );
		svg.render( &painter );
		p->drawPixmap(x,y,w,h,px);
	}
}


void PairingEditButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::paint(painter,option,index);
	if(0==index.column()){
		QRect r = option.rect;//getting the rect of the cell
		const qint32 border=2;
		const qint32 buttonSize=r.height()-border*2;

		QVariantMap data=index.data(Qt::DisplayRole).toMap();

		// Draw identicon to the far left
		PortableID id(data);
		qDebug()<<"ID: "<<id.id()<< " FROM DATA: "<<data;
		/*

FROM DATA:  QMap(("birthDate", QVariant(qulonglong, 1471476138556))
("createDate", QVariant(Invalid))
("gender", QVariant(QString, ""))
("key", QVariant(QVariantMap, QMap(("id", QVariant(QString, "53CD98E1996608801F960D46D86B15976FCB311B437142D943D2BDC65ACAD89C"))
("publicKey", QVariant(QString, "-----BEGIN PUBLIC KEY-----\nMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEAmVMIHQMoZ1TIuyYOBfVO\n4PwvR/Gc233ADpGezhCTKZ14zFl2MxxR167E0lVI4Qnl3L3LhKQgdRFEd2DiAoN+\nFId6fQuocGh/1PlV6tOBK7d2vVmuUrmHLoXi1gv8ZSN1BVTn64mBxWB7n57kDND9\nzoTqqICKsQ+7z2LTBryblOdisMcVVOY5a9ALokg/pgnpe0vDhX+342lhMYAaZfJP\nwxgCKJAcSTKhtAvdnnQXwkA1BjElvoq0UCB8SKo+oQ33h5w7unlyCsaJ07Q+U54g\nhZ5HvPKuDzZ/65wfB93MimJwM6Ew98FAqy73U9UZiLs3VrJ37x3EJAe39uIAMHKW\nlOx5Mu8yN9WWtzKqSX5F0k7Gs+c1MKk9lAcenJcsAqHNS9ND0x0NmAJVzanShan5\nsDx2V6J8cB5FssdAz301WxhnOi1sbfDJcy9dEpAHPs6g6FSYFBNAx1/9N4Yck1cR\n8M7JMxkCN9mo1BYbs8ON0leMGm/BtozgHpDGrDnTZ13fwNnODh9uSaZZeFvzQgDm\nHInb9+Wv8BoeSahlOKjsQV6Gqycb9qwZX3sJ/OK7X+hIDtc1yQ7Pdc5WN0zRuQvb\nu1uwlox6JAVXDhcrejweQgYI4garuRjfua0PEAqNk94v8bTvG+VPEYBdPRKiIDLp\ngEvoK4UpZOLXXmp1ao2lki8CAwEAAQ==\n-----END PUBLIC KEY-----\n")))))
("lastSeenMS", QVariant(qulonglong, 1471548235026))
("localAddress", QVariant(Invalid))
("name", QVariant(QString, ""))
("publicAddress", QVariant(QVariantMap, QMap(("ip", QVariant(QString, "0.0.0.0"))
("port", QVariant(int, 8123)))))
("role", QVariant(QString, "ROLE_CONTROL"))
("trusts", QVariant(QStringList, ("take-control")))
("type", QVariant(QString, "TYPE_REMOTE")))
*/
		Identicon ic(id);
		QPixmap px=ic.pixmap(buttonSize,buttonSize);

		// Draw state symbols in the middle
		quint32 icX=buttonSize+border*2;
		const qint64 now=QDateTime::currentMSecsSinceEpoch();

		const qint64 off=60*1000; //After a minute means M.I.A
		const qint64 full=10*1000; //Less than  10 sec means full-on
		const qint64 lastSeen=data["lastSeenMS"].toLongLong();
		const qint64 timeSinceLastSeen=now-lastSeen;

		qint64 t=timeSinceLastSeen-full;
		const qint64 step=(off-full)/5;
		int s=0;
		while(t>0.0 && s<5){
			t-=step;
			s++;
		}
		QString onlineSymbol="";
		switch(s){
			case(0):onlineSymbol=":/icons/connection_full.svg"; break;
			case(1):onlineSymbol=":/icons/connection_high.svg"; break;
			case(2):onlineSymbol=":/icons/connection_medium.svg"; break;
			case(3):onlineSymbol=":/icons/connection_low.svg"; break;
			default:
			case(4):onlineSymbol=":/icons/connection_off.svg"; break;
		}

		qDebug()<<"NOW="<<now<<", LastSeen="<<lastSeen<<", timeSinceLastSeen="<<timeSinceLastSeen<<", step="<<step<<", s="<<s<<", t="<<t<<", onlineSymbol="<<onlineSymbol;


		//Seen less than 20 sec ago means online
		//const QString onlineSymbol="";
		//(()<off) ?":/icons/connected.svg":":/icons/disconnected.svg";
		//Trust or not
		auto list=data["trusts"].toList();
		const QString trustSymbol=

				(list.contains("take-control") || list.contains("give-control"))
				?":/icons/trust.svg"
			   :(
					 list.contains("block")
					 ?":/icons/block.svg"
					:":/icons/ignore.svg"
					  );
		drawSVG(painter, onlineSymbol , icX,r.top()+border,buttonSize,buttonSize);
		icX+=buttonSize+border*2;
		drawSVG(painter, trustSymbol, icX,r.top()+border,buttonSize,buttonSize);
		icX+=buttonSize+border*2;
		//drawSVG(painter, , icX,border,buttonSize,buttonSize);
		//icX+=buttonSize+border*2;


		// Draw the edit button to the far right
		QStyleOptionButton button;
		int w = buttonSize*2;//button width
		int h = buttonSize;//button height
		int x = r.left() + r.width() - w - border*2;//the X coordinate
		int y = r.top()+border;//the Y coordinate

		painter->drawPixmap(QRect(border, y, buttonSize,buttonSize), px, QRect(0,0,buttonSize,buttonSize));

		button.rect = QRect(x,y,w,h);
		button.text.clear();
		button.state = QStyle::State_Enabled;
		button.iconSize=QSize(buttonSize*3/4,buttonSize*3/4);
		button.icon=QIcon(":/icons/edit.svg");
		button.state = option.state | QStyle::State_Enabled;
		QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);
	}
}

bool PairingEditButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if( 0==index.column() && event->type() == QEvent::MouseButtonRelease)
	{
		QMouseEvent * e = (QMouseEvent *)event;
		int clickX = e->x();
		int clickY = e->y();
		QRect r = option.rect;//getting the rect of the cell
		const int border=2;
		const int buttonSize=r.height()-border*2;
		int w = buttonSize*2;//button width
		int h = buttonSize;//button height
		int x = r.left() + r.width() - w - border*2;//the X coordinate
		int y = r.top()+border;//the Y coordinate

		/*
				int w = buttonSize*2;//button width
				int h = buttonSize;//button height
				int x = r.left() + r.width() - w;//the X coordinate
				int y = r.top();//the Y coordinate
*/
		if(( clickX > x && clickX < x + w ) && ( clickY > y && clickY < y + h ))
		{
			qDebug()<<"CLICK";
			pwiz.startEdit(index.row());
			return true;
		}
	}
	return false;
}

QSize PairingEditButtonDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const{
	return QSize(option.widget->size().width(),option.widget->size().height()*2/7);
}



