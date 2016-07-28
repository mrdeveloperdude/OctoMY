#include "PairingWizard.hpp"
#include "ui_PairingWizard.h"

#include "basic/Settings.hpp"
#include "basic/Node.hpp"
#include "widgets/Identicon.hpp"
#include "basic/NodeAssociate.hpp"
#include "security/PortableID.hpp"
#include "basic/Standard.hpp"

#include <QDebug>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QUrl>

#include <QWidget>
#include <QItemDelegate>
#include <QMouseEvent>
#include <QPainter>
#include <QSvgRenderer>

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
			//qDebug()<<"ORIG: "<<ds<<" ASPECT: "<<da<<" LOW: "<<low<<" HIGH: "<<high;
			const qreal zoom=(z<0)?0:(z>1)?1:z;
			const qreal izoom=1.0-zoom;
			return low*izoom+high*zoom;
		}



		QPixmap svgToPixmap(QString fn, const  qint32 w, const qint32 h, const qreal zoom=0.0) const
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


		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
		NodeAssociateStore &store;
		DiscoveryType typeFilter;
		PairingWizard &pwiz;


	private:
		bool filter(DiscoveryType &t) const{
			switch(typeFilter){
				case(TYPE_AGENT):return ((TYPE_REMOTE==t)||(TYPE_HUB==t));
				case(TYPE_REMOTE):return ((TYPE_HUB==t)||(TYPE_AGENT==t));
				case(TYPE_HUB):return ((TYPE_AGENT==t)||(TYPE_REMOTE==t)||(TYPE_HUB==t)); //Hubs are onmnivorus
				default:
					return false;
			}
			return false;
		}


	public:
		explicit PairingList(NodeAssociateStore &store, DiscoveryType typeFilter, PairingWizard &pwiz)
			: QAbstractListModel(&pwiz)
			, store(store)
			, typeFilter(typeFilter)
			, pwiz(pwiz)
		{
		}

		virtual ~PairingList()
		{
		}

	public:
		int rowCount(const QModelIndex &) const
		{
			int ret=0;
			QMap<QString, QSharedPointer<NodeAssociate> > participants=store.getParticipants();
			for(QMap<QString, QSharedPointer<NodeAssociate> >::const_iterator it=participants.begin(), e=participants.end(); it!=e;++it){
				QSharedPointer<NodeAssociate> p=it.value();
				if(nullptr!=p){
					DiscoveryType t=p->type();
					if(filter(t)){
						ret++;
					}
				}
			}
			//qDebug()<<"PairingList Rowcount: "<<ret;
			return ret;
		}

		int columnCount(const QModelIndex &) const
		{
			return 1;
		}

		QVariant data(const QModelIndex &index, int role) const
		{
			QMap<QString, QSharedPointer<NodeAssociate> > &participants=store.getParticipants();
			int targetRow=index.row();
			int rowCounter=0;
			QSharedPointer<NodeAssociate> part;

			for(QMap<QString, QSharedPointer<NodeAssociate> >::const_iterator it=participants.begin(), e=participants.end(); it!=e;++it){
				QSharedPointer<NodeAssociate> p=it.value();
				if(nullptr!=p){
					DiscoveryType t=p->type();
					if(filter(t)){
						if(rowCounter==targetRow){
							part=p;
							break;
						}
						rowCounter++;
					}
				}
			}

			if (nullptr!=part) {
				DiscoveryType t=part->type();
				if(filter(t)){
					if(Qt::DisplayRole == role ) {
						return part->toVariantMap();
					}
					else if (Qt::ToolTipRole == role){
						return DiscoveryTypeToString(t) +": " +part->id();
					}
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
	, mNode(nullptr)
	, mList(nullptr)
	, mDelegate (nullptr)

{
	ui->setupUi(this);
	mTemplate=ui->labelBodyPair->text();
	ui->labelBodyPair->setText("<h1>Please wait...<h1>");

	reset();

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
		},OC_CONTYPE);
	}
}


PairingWizard::~PairingWizard()
{
	delete ui;
}


void PairingWizard::configure(Node *n)
{
	mNode=n;
	if(nullptr!=mNode){
		DiscoveryClient *discovery=mNode->discoveryClient();
		DiscoveryType type=mNode->type();
		PortableID pid=mNode->keyStore().localPortableID();
		pid.setType(type);
		ui->widgetMyCertificate->setPortableID(pid);
		if(!connect(discovery, &DiscoveryClient::nodeDiscovered, [=](QString partID)
		{
					if(nullptr==ui->listViewNodes->model()){

					mList=new PairingList(mNode->peers(),type,*this);
					ui->listViewNodes->setModel(mList);

					if(nullptr==mDelegate){
					mDelegate=new EditButtonDelegate(*this);
	}
					//ui->tableViewNodes->setItemDelegate(delegate);
					ui->listViewNodes->setItemDelegate(mDelegate);

					//qDebug()<<"SET TABLE MODEL";
	}

					//qDebug()<<"PAIRING WIZARD partID: "<<partID;
					ui->listViewNodes->update();
	})){
			qWarning()<<"ERROR: Could not connect";
		}



		switch(type){
			default:
			case(TYPE_ZOO):
			case(TYPE_UNKNOWN):
			case(TYPE_AGENT):{
					ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Agent").replace(QRegularExpression("\\[DEST\\]"), "Control"));
					ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageAgent);
					ui->checkBoxBlock->setVisible(true);
					ui->checkBoxControl->setVisible(true);
					ui->checkBoxTrustNode->setVisible(false);

				}break;
			case(TYPE_REMOTE):{
					ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Remote").replace(QRegularExpression("\\[DEST\\]"), "Agent"));
					ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageControl);
					ui->checkBoxBlock->setVisible(true);
					ui->checkBoxControl->setVisible(false);
					ui->checkBoxTrustNode->setVisible(true);

				}break;
			case(TYPE_HUB):{
					ui->labelBodyPair->setText(mTemplate.replace(QRegularExpression("\\[SOURCE\\]"), "Hub").replace(QRegularExpression("\\[DEST\\]"), "Agent"));
					ui->stackedWidgetNoMessage->setCurrentWidget(ui->pageNoMessageControl);
					ui->checkBoxBlock->setVisible(true);
					ui->checkBoxControl->setVisible(true);
					ui->checkBoxTrustNode->setVisible(true);

				}break;

		}

	}
}


void PairingWizard::reset()
{
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
}


void PairingWizard::startEdit(int row)
{
	qDebug()<<"STARTING EDIT FOR "<<row;
	QModelIndex  index=mList->index(row,0);
	if(index.isValid()){
		setUpdatesEnabled(false);
		QVariantMap map=index.data(Qt::DisplayRole).toMap();
		//qDebug()<<"DATA FOR "<<row<<": "<<map;
		PortableID id(map);
		ui->widgetParticipantCertificate->setPortableID(id);
		ui->stackedWidget->setCurrentWidget(ui->pagePeerDetail);
		setUpdatesEnabled(true);
	}
	else{
		qWarning()<<"ERROR: Index was invalid for row "<<row;
	}
}

Node *PairingWizard::getNode()
{
	return mNode;
}

void PairingWizard::showEvent(QShowEvent *)
{
	if(nullptr!=mNode){
		DiscoveryClient *client=mNode->discoveryClient();
		if(nullptr!=client){
			client->start();
		}
	}
}

void PairingWizard::hideEvent(QHideEvent *)
{
	if(nullptr!=mNode){
		DiscoveryClient *client=mNode->discoveryClient();
		if(nullptr!=client){
			client->stop();
		}
	}
}

void PairingWizard::on_pushButtonMaybeOnward_clicked()
{
	if(nullptr!=mNode){
		NodeAssociateStore &store=mNode->peers();
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
	emit done();
}


void PairingWizard::on_pushButtonCameraPair_clicked()
{
	ui->stackedWidget->setCurrentWidget(ui->pageCameraPairing);
}

void PairingWizard::on_pushButtonDiscoverNow_clicked()
{
	if(nullptr!=mNode){
		DiscoveryClient *client=mNode->discoveryClient();
		if(nullptr!=client){
			client->discover();
		}
	}
}


void PairingWizard::on_pushButtonSaveEdits_clicked()
{
	//TODO: Save changes
	ui->stackedWidget->setCurrentWidget(ui->pagePairWithPeers);
	const bool block=ui->checkBoxBlock->isChecked();
	const bool controlThem=ui->checkBoxControl->isChecked();
	const bool trustThem=ui->checkBoxTrustNode->isChecked();
}
