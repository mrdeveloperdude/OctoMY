#include "PlanBookService.hpp"


#include "node/Node.hpp"



PlanBookService::PlanBookService(QSharedPointer<PlanBook> planBook, QStringList dependencies)
	: ServiceWrapper<PlanBook>(planBook, "PlanBook", dependencies)
	, mConfigureHelper("PlanBookService", true, true, false, Constants::OC_LOG_CONFIGURE_HELPER_WARNINGS, Constants::OC_LOG_CONFIGURE_HELPER_CHANGES)
{
	OC_METHODGATE();
}


void PlanBookService::configure(QSharedPointer<Node> node)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		mNode=node;
	}
}


void PlanBookService::serviceWrapperActivate(QSharedPointer<PlanBook> planBook, bool on, ServiceActivatedCallback callBack)
{
	OC_METHODGATE();
	if(mConfigureHelper.activate(on)) {
		planBook->activate(on, [this, planBook, callBack, on](bool ok) {
			if(!planBook.isNull()) {
				//qDebug()<<"PlanBook synchronized with ok="<<ok;
				auto map=planBook->toMap();
				//mNode->cl
				qDebug() << "plan book service " << on << map;
				
			} else {
				qWarning()<<"ERROR: plan book not ok";
			}
			if(nullptr != callBack) {
				callBack(on, ok);
			}
		});
	}
}
