#include "AgentItemDelegate.hpp"

#include "identity/Identicon.hpp"
#include "security/PortableID.hpp"

#include "uptime/MethodGate.hpp"
#include "utility/time/HumanTime.hpp"
#include "utility/graphics/Graphics.hpp"

#include <QSvgRenderer>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>


AgentItemDelegate::AgentItemDelegate(QObject *parent)
	: QItemDelegate(parent)
{
	OC_METHODGATE();
	//Q_INIT_RESOURCE(icons);
}


AgentItemDelegate::~AgentItemDelegate()
{
	OC_METHODGATE();
}


static QSizeF calcSize(const QSizeF &ds, const qint32 w, const qint32 h, qreal const z)
{
	OC_FUNCTIONGATE();
	if(w < 1 && h < 1) {
		return ds;
	}
	const qreal da=ds.width()/ds.height();
	//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
	QSizeF low(w,static_cast<qreal>(h / da));
	QSizeF high(static_cast<qreal>(w * da) , h);
	//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
	const qreal zoom = ( z < 0 ) ? 0 : ( z > 1 ) ? 1 : z;
	const qreal izoom = 1.0 - zoom;
	return low * izoom + high * zoom;
}


static void drawSVG(QPainter &painter, const QString url, const qint32 x, const qint32 y, const qint32 w, const qint32 h, const qreal zoom=0.0)
{
	OC_FUNCTIONGATE();
	QSvgRenderer svg(url);
	QSizeF ds = calcSize(svg.defaultSize(),w,h,zoom);
	QPixmap px(ds.toSize());
	px.fill(QColor(0, 0, 0, 0));
	QPainter p( &px );
	svg.render( &p );
	painter.drawPixmap(x, y, w, h, px);
}


static void drawTrustSymbol(QPainter &painter, const QVariantList &trust, const QRect &r, int &icX, const qint32 &border, const qint32 &buttonSize){
	OC_FUNCTIONGATE();
	const QString trustSymbol=
	(trust.contains("take-control") || trust.contains("give-control"))
	?":/icons/trust.svg"
	:(
		  trust.contains("block")
			  ?":/icons/block.svg"
			  :":/icons/ignore.svg"
		  );
	drawSVG(painter, trustSymbol, static_cast<int>(icX), r.top()+border, buttonSize, buttonSize);
	icX+=static_cast<quint32>(buttonSize+border*2);
}


static void drawButton(QPainter &painter, const QRect &r, int &icX, const qint32 &border, const qint32 &buttonSize, const QIcon &icon, const QStyle::State state){
	QStyleOptionButton button;
	int x{0};
	int w = buttonSize*2;//button width
	if(icX>0){
		x = r.left() + border + icX-1;
		icX += w;
	}
	else{
		x = r.left() + r.width() - w - border + icX;
		icX -= w;
	}
	
	int h = buttonSize;//button height
	int y = r.top()+border;//the Y coordinate
	button.rect = QRect(x, y, w, h);
	button.text.clear();
	button.state = QStyle::State_Enabled;
	button.iconSize = QSize(buttonSize*3/4, buttonSize*3/4);
	button.icon = icon;
	button.state = state;
	QApplication::style()->drawControl( QStyle::CE_PushButton, &button, &painter);
}

static void drawText(QPainter &painter, const QRect &r, int &icX, const qint32 &buttonSize, QPen &pen, const QString &text){
	// Draw the identifier
	painter.setPen(pen);
	auto font = painter.font();
	font.setPixelSize(buttonSize/2);
	painter.setFont(font);
	utility::graphics::drawText(painter, QPointF(icX, r.top()+r.height()/2), Qt::AlignVCenter, text);
	
}


void AgentItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	OC_METHODGATE();
	QItemDelegate::paint(painter, option, index);
	if(0 == index.column()  && false) {
		auto r = option.rect;
		const qint32 border{2};
		const qint32 buttonSize{r.height()-border * 2};
		
		QVariantMap data=index.data(Qt::DisplayRole).toMap();
		auto id = data["id"].toString();
		auto name = data["name"].toString();
		
		int icX{1};
		//drawSVG(*painter, id, icX, r.top()+border, buttonSize, buttonSize);
				
		// Draw state symbols in the middle
		icX = buttonSize+border*2;
		
		
		drawTrustSymbol(*painter, data["trusts"].toList(), r, icX, border, buttonSize);
		
		// Draw the identifier
		auto widg = qobject_cast<QWidget *>(parent());
		QPen pen;
		if(nullptr != widg) {
			pen = widg->palette().color(QPalette::ButtonText);
		}
		drawText(*painter, r, icX, buttonSize, pen, name);
		// Draw the edit button to the far right
		icX = 0;
		drawButton(*painter, r, icX, border, buttonSize, QIcon(":/icons/control.svg"), option.state | QStyle::State_Enabled);
		drawButton(*painter, r, icX, border, buttonSize, QIcon(":/icons/app/edit.svg"), option.state | QStyle::State_Enabled);
	}
}


bool AgentItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
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
		const auto buttonW = buttonSize * 2 + border;
		const auto buttonH = buttonSize;
		const auto buttonX1 = r.left() + r.width() - buttonW - border;
		const auto buttonX2 = buttonX1 - buttonW;
		const auto buttonY = r.top() + border;
		if( clickY > buttonY && clickY < buttonY + buttonH ){
			if( clickX > buttonX1 && clickX < buttonX1 + buttonW ) {
				auto data = index.data(Qt::DisplayRole).toMap();
				PortableID id(data);
				if(mDebug){
					qDebug() << "CLICK 1" << id;
				}
				emit startControl(id.id());
				return true;
			}
			else  if( clickX > buttonX2 && clickX < buttonX2 + buttonW ) {
				auto data = index.data(Qt::DisplayRole).toMap();
				PortableID id(data);
				if(mDebug){
					qDebug() << "CLICK 2" << id;
				}
				emit startEdit(id.id());
				return true;
			}
		}
	}
	return false;
}


QSize AgentItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	OC_METHODGATE();
	Q_UNUSED(index);
	return QSize(option.widget->size().width(), 38);
}
