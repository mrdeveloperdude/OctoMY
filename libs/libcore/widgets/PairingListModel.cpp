#include "PairingListModel.hpp"

#include "PairingWizard.hpp"
#include "basic/Settings.hpp"
#include "basic/Node.hpp"
#include "widgets/Identicon.hpp"
#include "basic/NodeAssociate.hpp"
#include "security/PortableID.hpp"
#include "../libutil/utility/Standard.hpp"

#include <QDebug>
#include <QAbstractListModel>

		bool PairingListModel::filter(DiscoveryType &t) const{
			switch(typeFilter){
				case(TYPE_AGENT):return ((TYPE_REMOTE==t)||(TYPE_HUB==t));
				case(TYPE_REMOTE):return ((TYPE_HUB==t)||(TYPE_AGENT==t));
				case(TYPE_HUB):return ((TYPE_AGENT==t)||(TYPE_REMOTE==t)||(TYPE_HUB==t)); //Hubs are onmnivorus
				default:
					return false;
			}
			return false;
		}


		PairingListModel::PairingListModel(NodeAssociateStore &store, DiscoveryType typeFilter, PairingWizard &pwiz)
			: QAbstractListModel(&pwiz)
			, store(store)
			, typeFilter(typeFilter)
			, pwiz(pwiz)
		{
		}

		PairingListModel::~PairingListModel()
		{
		}


		int PairingListModel::rowCount(const QModelIndex &) const
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

		int PairingListModel::columnCount(const QModelIndex &) const
		{
			return 1;
		}

		QVariant PairingListModel::data(const QModelIndex &index, int role) const
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


		QVariant PairingListModel::headerData(int section, Qt::Orientation orientation, int role) const
		{
			return QVariant();
		}

