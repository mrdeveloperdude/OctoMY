#include "IdenticonLabel.hpp"

#include "uptime/MethodGate.hpp"
#include "identity/IdenticonTooltip.hpp"

IdenticonLabel::IdenticonLabel(QWidget *parent)
	: QLabel(parent)
{
	OC_METHODGATE();
}


void IdenticonLabel::setPortableID(PortableID &id, bool showName, bool showIcon)
{
	OC_METHODGATE();
	mIdenticon.setPortableID(id);
	setPixmap(showIcon?mIdenticon.pixmap(32, 32):QPixmap());
	setText(showName?id.name():QString());
	setToolTip(identiconTooltip(id));
}


Identicon IdenticonLabel::identicon()
{
	OC_METHODGATE();
	return mIdenticon;
}
