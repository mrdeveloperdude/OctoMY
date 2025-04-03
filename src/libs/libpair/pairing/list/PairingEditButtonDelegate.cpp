#include "PairingEditButtonDelegate.hpp"

#include "../PairingMenuActivity.hpp"
#include "identity/Identicon.hpp"
#include "security/PortableID.hpp"

#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"
#include "utility/graphics/Graphics.hpp"

#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>


PairingEditButtonDelegate::PairingEditButtonDelegate(QObject *parent)
	: QItemDelegate(parent)
{
	OC_METHODGATE();
	//Q_INIT_RESOURCE(icons);
}


PairingEditButtonDelegate::~PairingEditButtonDelegate()
{
	OC_METHODGATE();
}


static QSizeF calcSize(const QSizeF &ds, const qint32 w, const qint32 h, qreal const z)
{
	OC_FUNCTIONGATE();
	if(w<1 && h<1) {
		return ds;
	}
	const qreal da=ds.width()/ds.height();
	//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
	QSizeF low(w,static_cast<qreal>(h/da));
	QSizeF high(static_cast<qreal>(w*da),h);
	//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	const qreal zoom=(z<0)?0:(z>1)?1:z;
	const qreal izoom=1.0-zoom;
	return low*izoom+high*zoom;
}


static void drawSVG(QPainter *p, QString url, qint32 x,qint32 y, qint32 w,qint32 h, qreal zoom=0.0)
{
	OC_FUNCTIONGATE();
	if(nullptr!=p) {
		QSvgRenderer svg(url);
		QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
		QPixmap px(ds.toSize());
		px.fill(QColor(0,0,0,0));
		QPainter painter( &px );
		svg.render( &painter );
		p->drawPixmap(x,y,w,h,px);
	}
}


static QString connectionStrengthToSymbol(int s)
{
	QString onlineSymbol="";
	switch(s) {
	case(0):
		onlineSymbol=":/icons/comms/connection_full.svg";
		break;
	case(1):
		onlineSymbol=":/icons/comms/connection_high.svg";
		break;
	case(2):
		onlineSymbol=":/icons/comms/connection_medium.svg";
		break;
	case(3):
		onlineSymbol=":/icons/comms/connection_low.svg";
		break;
	default:
	case(4):
		onlineSymbol=":/icons/comms/connection_off.svg";
		break;
	}
	return onlineSymbol;
}


static int calculateConnectionStrength(qint64 lastSeen)
{
	const qint64 now=utility::time::currentMsecsSinceEpoch<qint64>();

	const qint64 off = 60 * 1000;  // After a minute means M.I.A
	const qint64 full = 10 * 1000; // Less than  10 sec means full-on
	const qint64 timeSinceLastSeen = now - lastSeen;

	qint64 t=timeSinceLastSeen-full;
	const qint64 step=(off-full)/5;
	int s=0;
	while(t>0.0 && s<5) {
		t-=step;
		s++;
	}
	// qDebug().noquote().nospace()<<"NOW="<<now<<", LastSeen="<<lastSeen<<", timeSinceLastSeen="<<timeSinceLastSeen<<", step="<<step<<", s="<<s<<", t="<<t;
	return s;
}


void PairingEditButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	OC_METHODGATE();
	QItemDelegate::paint(painter, option, index);
	if(0 == index.column()) {
		QRect r = option.rect;//getting the rect of the cell
		const qint32 border=2;
		const qint32 buttonSize=r.height()-border*2;

		QVariantMap data=index.data(Qt::DisplayRole).toMap();

		// Draw identicon to the far left
		PortableID id(data);
		//qDebug()<<"ID: "<<id.id()<< " FROM DATA: "<<data;

		Identicon ic(id);
		QPixmap px=ic.pixmap(buttonSize,buttonSize);

		// Draw state symbols in the middle
		quint32 icX=static_cast<quint32>(buttonSize+border*2);

		qint64 lastSeenMS=static_cast<qint64>(utility::time::variantToMs(data["lastSeenMS"]));
		//qDebug().noquote().nospace()<<"data[\"lastSeenMS\"]="<<data["lastSeenMS"]<<" converted to "<<lastSeenMS;
		QString onlineSymbol=connectionStrengthToSymbol(calculateConnectionStrength(lastSeenMS));

		//Trust or not
		auto list = data["trusts"].toList();
		const QString trustSymbol=

			(list.contains("take-control") || list.contains("give-control"))
			?":/icons/pairing/trust.svg"
			:(
				list.contains("block")
				?":/icons/block.svg"
				:":/icons/ignore.svg"
			);
		drawSVG(painter, onlineSymbol, static_cast<int>(icX), r.top()+border, buttonSize, buttonSize);
		icX+=static_cast<quint32>(buttonSize+border*2);
		drawSVG(painter, trustSymbol, static_cast<int>(icX), r.top()+border, buttonSize, buttonSize);
		icX+=static_cast<quint32>(buttonSize+border*2);
		//drawSVG(painter, , icX,border,buttonSize,buttonSize);
		//icX+=buttonSize+border*2;

/*
		QString identifier=data["name"].toString().trimmed();
		if(identifier.isEmpty()) {
			identifier=data["type"].toString().trimmed().mid(5)+"-"+id.id().left(10)+"...";
		}
		*/
		
		// Draw the identifier
		auto widg = qobject_cast<QWidget *>(parent());
		if(nullptr != widg) {
			painter->setPen(widg->palette().color(QPalette::ButtonText));
		}
		auto font = painter->font();
		font.setPixelSize(buttonSize/2);
		painter->setFont(font);
		utility::graphics::drawText(*painter, QPointF(icX, r.top()+r.height()/2), Qt::AlignVCenter, id.identifier());

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
		button.icon=QIcon(":/icons/app/edit.svg");
		button.state = option.state | QStyle::State_Enabled;
		QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);
	}
}


bool PairingEditButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	OC_METHODGATE();
	Q_UNUSED(model);
	//qDebug() << "EVENT" << event;
	if(0 == index.column() && event->type() == QEvent::MouseButtonRelease) {
		const auto e = static_cast<QMouseEvent *>(event);
		const auto pos = e->position();
		const auto clickX = pos.x();
		const auto clickY = pos.y();
		const auto r = option.rect; //getting the rect of the cell
		static const int border = 2;
		const int buttonSize = r.height() - border*2;
		const auto buttonW = buttonSize * 2;
		const auto buttonH = buttonSize;
		const auto buttonX = r.left() + r.width() - buttonW - border * 2;
		const auto buttonY = r.top() + border;
		if(( clickX > buttonX && clickX < buttonX + buttonW ) && ( clickY > buttonY && clickY < buttonY + buttonH )) {
			auto data = index.data(Qt::DisplayRole).toMap();
			PortableID id(data);
			if(mDebug){
				qDebug() << "CLICK" << id;
			}
			emit startEdit(id.id());
			return true;
		}
	}
	return false;
}


QSize PairingEditButtonDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	OC_METHODGATE();
	Q_UNUSED(index);
	return QSize(option.widget->size().width(), 38);
}
