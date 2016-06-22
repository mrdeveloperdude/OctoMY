#include "PairingWizard.hpp"
#include "ui_PairingWizard.h"

#include "basic/Settings.hpp"
#include "basic/Node.hpp"
#include "widgets/Identicon.hpp"
#include "comms/discovery/DiscoveryParticipant.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>

#include <QWidget>
#include <QItemDelegate>
#include <QMouseEvent>
#include <QPainter>

// QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=mTiqZm-Ea70", QUrl::TolerantMode));


class EditButtonDelegate: public QItemDelegate
{
		//Q_OBJECT

	private:

		//const int buttonSize=30;
		PairingWizard &pwiz;
	public:
		explicit EditButtonDelegate(PairingWizard &pwiz)
			: QItemDelegate(&pwiz)
			, pwiz(pwiz)
		{
		}
		virtual ~EditButtonDelegate()
		{

		}



		static QSizeF calcSize(QSizeF ds, const qint32 w, const qint32 h, qreal const z=0.0)
		{
			if(w<1 && h<1){
				return ds;
			}
			const qreal da=ds.width()/ds.height();
			//	const qreal ca=((qreal)(w<1?1:w)/(qreal)(h<1?1:h));
			QSizeF low(w,(qreal)h/da);
			QSizeF high((qreal)w*da,h);
			qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
			const qreal zoom=(z<0)?0:(z>1)?1:z;
			const qreal izoom=1.0-zoom;
			return low*izoom+high*zoom;
		}



		QPixmap svgToPixmap(QString fn, const  qint32 w, const qint32 h, const qreal zoom=0.0) const
		{
			QSvgRenderer svg(fn);
			QSizeF ds=calcSize(svg.defaultSize(),w,h,zoom);
			qDebug()<<"Generating pixmap from identicon with size: "<<ds;
			QPixmap px(ds.toSize());
			px.fill(QColor(0,0,0,0));
			QPainter painter( &px );
			svg.render( &painter );
			//px.save("/tmp/px.png");
			return px;
		}


		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
		{
			QItemDelegate::paint(painter,option,index);
			if(0==index.column()){
				QRect r = option.rect;//getting the rect of the cell
				const qint32 border=2;
				const qint32 buttonSize=r.height()-border*2;


				QVariantMap data=index.data(Qt::DisplayRole).toMap();

				QPixmap px;
				const QString type=data["type"].toString();
				if("TYPE_AGENT"==type){
					Identicon id(QStringLiteral(":/icons/identicon.svg"), data["ID"].toByteArray());
					px=id.pixmap(buttonSize,buttonSize);
				}
				else if ("TYPE_REMOTE"==type){
					px=svgToPixmap(":/icons/remote.svg", buttonSize, buttonSize, 0.0);
				}
				else if ("TYPE_HUB"==type){
					px=svgToPixmap(":/icons/hub.svg", buttonSize, buttonSize, 0.0);
				}
				else{
					qWarning()<<"ERROR: Unknown type "<<type<<" when rendering peer in delegate. Data was "<<data;
				}

				painter->drawPixmap(QRect(border, border, buttonSize,buttonSize), px, QRect(0,0,buttonSize,buttonSize));


				QStyleOptionButton button;
				int w = buttonSize*2;//button width
				int h = buttonSize;//button height
				int x = r.left() + r.width() - w - border*2;//the X coordinate
				int y = r.top()+border;//the Y coordinate
				button.rect = QRect(x,y,w,h);
				button.text.clear();
				button.state = QStyle::State_Enabled;
				button.iconSize=QSize(buttonSize*3/4,buttonSize*3/4);
				button.icon=QIcon(":/icons/edit.svg");
				button.state = option.state | QStyle::State_Enabled;
				QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);
			}
		}

		bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
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

		QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE{
			return QSize(option.widget->size().width(),option.widget->size().height()*2/7);
		}

};



class PairingList: public QAbstractListModel
{
	private:
		DiscoveryClientStore &store;
		DiscoveryRole roleFilter;
		PairingWizard &pwiz;


	public:
		explicit PairingList(DiscoveryClientStore &store, DiscoveryRole roleFilter, PairingWizard &pwiz)
			: QAbstractListModel(&pwiz)
			, store(store)
			, roleFilter(roleFilter)
			, pwiz(pwiz)
		{
		}

		virtual ~PairingList()
		{
		}


		int rowCount(const QModelIndex &) const
		{
			int ret=0;
			QMap<QString, DiscoveryParticipant *> &participants=store.getParticipants();
			for(QMap<QString, DiscoveryParticipant *>::const_iterator it=participants.begin(), e=participants.end(); it!=e;++it){
				DiscoveryParticipant *p=it.value();
				if(nullptr!=p){
					if(roleFilter==p->role){
						ret++;
					}
				}
			}
			qDebug()<<"rowcount: "<<ret;
			return ret;
		}

		int columnCount(const QModelIndex &) const
		{
			return 1;
		}

		QVariant data(const QModelIndex &index, int role) const
		{
			QMap<QString, DiscoveryParticipant *> &participants=store.getParticipants();
			QMap<QString, DiscoveryParticipant *>::const_iterator it = participants.begin();
			it+=index.row();
			DiscoveryParticipant *part=it.value();

			if (nullptr!=part) {
				if(Qt::DisplayRole == role ) {
					return part->toVariantMap();
				}
				else if (Qt::ToolTipRole == role){
					return DiscoveryTypeToString(part->type) +": " +part->ID;
				}
			}
			return QVariant();
		}


		QVariant headerData(int section, Qt::Orientation orientation, int role) const
		{
			return QVariant();
		}

};


//////////////////////
PairingWizard::PairingWizard(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PairingWizard)
	, node(nullptr)
	, list(nullptr)
	, delegate (nullptr)

{
	ui->setupUi(this);

	reset();
}

PairingWizard::~PairingWizard()
{
	delete ui;
}


void PairingWizard::configure(Node *n)
{
	node=n;
	if(nullptr!=node){
		DiscoveryClient *discovery=node->getDiscoveryClient();
		DiscoveryRole role=node->getRole();

		if(!connect(discovery, &DiscoveryClient::nodeDiscovered, [=](QString partID)
		{
					if(nullptr==ui->listViewNodes->model()){

					list=new PairingList(node->getPeers(),ROLE_AGENT==role?ROLE_CONTROL:ROLE_AGENT,*this);
					ui->listViewNodes->setModel(list);

					if(nullptr==delegate){
					delegate=new EditButtonDelegate(*this);
	}
					//ui->tableViewNodes->setItemDelegate(delegate);
					ui->listViewNodes->setItemDelegate(delegate);

					qDebug()<<"SET TABLE MODEL";
	}

					qDebug()<<"PW: partID: "<<partID;
					ui->listViewNodes->update();
	})){
			qWarning()<<"ERROR: Could not connect";
		}



		switch(role){
			default:
			case(ROLE_UNKNOWN):
			case(ROLE_AGENT):{
					ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageAgent);
					ui->stackedWidgetPairMessage->setCurrentWidget(ui->pagePairMessageAgent);
				}break;
			case(ROLE_CONTROL):{
					ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageControl);
					ui->stackedWidgetPairMessage->setCurrentWidget(ui->pagePairMessageControl);
				}break;

		}

		// Hook onward buttons to go to the correct page in stack
		QList<QPushButton *> onwardButtons = ui->stackedWidget->findChildren<QPushButton *>(QRegularExpression("pushButtonOnward.*"));
		//qDebug()<<"FOUND "<<onwardButtons.size()<<" ONWARDs";
		for (QList<QPushButton*>::iterator it = onwardButtons.begin(), e=onwardButtons.end(); it != e; ++it) {
			QPushButton*onward=(*it);
			//qDebug()<<" + ONWARD: "<<onward->objectName();
			connect(onward, &QPushButton::clicked,this,[=](bool b){
				// Skip pages that are not relevant to the selection made in "basic" page
				int next = (ui->stackedWidget->currentIndex() + 1) % ui->stackedWidget->count();
				ui->stackedWidget->setCurrentIndex(next);
			});
		}
	}
}


void PairingWizard::reset(){
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
}


void PairingWizard::startEdit(int row){
	qDebug()<<"STARTING EDIT FOR "<<row;
	ui->stackedWidget->setCurrentWidget(ui->pagePeerDetail);
	ui->widgetIdenticon->setIdenticonData("LOL");
	ui->widgetQR->setQRData(""+row);
}

Node *PairingWizard::getNode(){
	return node;
}

void PairingWizard::showEvent(QShowEvent *){
	if(nullptr!=node){
		DiscoveryClient *client=node->getDiscoveryClient();
		if(nullptr!=client){
			client->start();
		}
	}
}

void PairingWizard::hideEvent(QHideEvent *){
	if(nullptr!=node){
		DiscoveryClient *client=node->getDiscoveryClient();
		if(nullptr!=client){
			client->stop();
		}
	}
}

void PairingWizard::on_pushButtonMaybeOnward_clicked()
{
	if(nullptr!=node){
		DiscoveryClientStore &store=node->getPeers();
		if(store.getParticipants().size()>0){
			emit done();
			return;
		}
	}
	ui->stackedWidget->setCurrentWidget(ui->pageNoPeers);
}


void PairingWizard::on_pushButtonTryAgain_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
}

void PairingWizard::on_pushButtonDone_clicked()
{
	//pm.stop();//Not necessary, well handled by hide event handler above
	emit done();
}


void PairingWizard::on_pushButtonCameraPair_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCameraPairing);
}

void PairingWizard::on_pushButtonDiscoverNow_clicked()
{
	if(nullptr!=node){
		DiscoveryClient *client=node->getDiscoveryClient();
		if(nullptr!=client){
			client->discover();
		}
	}
}


void PairingWizard::on_pushButtonSaveEdits_clicked()
{
	//TODO: Save changes
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
}
