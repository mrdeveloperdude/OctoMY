#include "TrustList.hpp"

#include "uptime/MethodGate.hpp"

const QString TrustList::giveControl("give-control");
const QString TrustList::takeControl("take-control");
const QString TrustList::block("block");


TrustList::TrustList()
	: QSet<QString>()
{
	OC_METHODGATE();
}


TrustList::TrustList(const QSet<QString> &l)
	: QSet<QString>(l)
{
	OC_METHODGATE();
}


TrustList::TrustList(const QStringList &l)
{
	OC_METHODGATE();
	for(const QString &s:l) {
		*this += s;
	}
}


void TrustList::clearTrust()
{
	OC_METHODGATE();
	clear();
}


void TrustList::addTrust(QString trust)
{
	OC_METHODGATE();
	*this+=trust;
}


void TrustList::addTrusts(QSet<QString> trusts)
{
	OC_METHODGATE();
	*this+=trusts;
}


void TrustList::removeTrust(QString trust)
{
	OC_METHODGATE();
	if(contains(trust)) {
		remove(trust);
	}
}


bool TrustList::hasTrust(QString trust)
{
	OC_METHODGATE();
	return contains(trust);
}


void TrustList::applyTrustLevel(TrustLevel level, NodeType type)
{
	OC_METHODGATE();

	// Start with clean slate
	removeTrust(giveControl);
	removeTrust(takeControl);
	removeTrust(block);
	switch(level) {
		case(TrustLevel::BLOCK): {
			addTrust(block);
		}
		break;
		case(TrustLevel::TRUST): {
	
			switch(type) {
	//				default:
			case(TYPE_ZOO):
			case(TYPE_UNKNOWN):
			case(TYPE_AGENT): {
				addTrust(giveControl);
			}
			break;
			case(TYPE_REMOTE): {
				addTrust(takeControl);
			}
			break;
			case(TYPE_HUB): {
				addTrust(takeControl);
				addTrust(giveControl);
			}
			break;
			}
		}
		break;
		default:
		case(TrustLevel::IGNORE): {
	// No trusts will be added for ignore
		} break;

	}
}


TrustLevel TrustList::toTrustLevel(NodeType type)
{
	OC_METHODGATE();
	const bool take=contains(takeControl);
	const bool give=contains(giveControl);
	const bool blk=contains(block);
	if(blk) {
		return TrustLevel::BLOCK;
	} else {
		switch(type) {
//					default:
		case(TYPE_ZOO):
		case(TYPE_UNKNOWN):
		case(TYPE_AGENT): {
			if(give) {
				return TrustLevel::TRUST;
			}
		}
		break;
		case(TYPE_REMOTE): {
			if(take) {
				return TrustLevel::TRUST;
			}
		}
		break;
		case(TYPE_HUB): {
			if(give || take) {
				return TrustLevel::TRUST;
			}
		}
		break;
		}
	}
	return TrustLevel::IGNORE;
}



QStringList TrustList::toStringList() const
{
	OC_METHODGATE();
	return QStringList(begin(), end());
}
