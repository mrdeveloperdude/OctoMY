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


void PairingEditButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QItemDelegate::paint(painter,option,index);
	if(0==index.column()){
		QRect r = option.rect;//getting the rect of the cell
		const qint32 border=2;
		const qint32 buttonSize=r.height()-border*2;

		QVariantMap data=index.data(Qt::DisplayRole).toMap();

		PortableID id(data);
		//qDebug()<<"ID: "<<id.id()<< " FROM DATA: "<<data;
		Identicon ic(id);
		QPixmap px=ic.pixmap(buttonSize,buttonSize);

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



