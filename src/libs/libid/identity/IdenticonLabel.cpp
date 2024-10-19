#include "IdenticonLabel.hpp"

#include <QLabel>
#include <QBoxLayout>


#include "uptime/MethodGate.hpp"
#include "identity/IdenticonTooltip.hpp"

IdenticonLabel::IdenticonLabel(QWidget *parent)
	: QWidget(parent)
	, mIconLabel(new QLabel(this))
	, mTextLabel(new QLabel(this))
	, mLayout(new QHBoxLayout(this))
{
	OC_METHODGATE();
	mLayout->addWidget(mIconLabel);
	mLayout->addWidget(mTextLabel);
	mLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(mLayout);
}


void IdenticonLabel::setPortableID(PortableID &id, bool showName, bool showIcon)
{
	OC_METHODGATE();
	mIdenticon.setPortableID(id);
	
	mIconLabel->setPixmap(showIcon ? mIdenticon.pixmap(32, 32) : QPixmap());
	mIconLabel->setVisible(showIcon);
	
	mTextLabel->setText(showName ? id.identifier() : QString());
	mTextLabel->setVisible(showName);
	
	setToolTip(identiconTooltip(id));
	/*
	QSizePolicy sp(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sp.setRetainSizeWhenHidden(true); // Ensures space for icon
	setSizePolicy(sp);
*/
}


Identicon IdenticonLabel::identicon()
{
	OC_METHODGATE();
	return mIdenticon;
}



QSize IdenticonLabel::minimumSizeHint() const
{
	int width = 0;
	int height = 0;
	auto px = mIconLabel->pixmap();
	if (mIconLabel->isVisible() && !px.isNull()) {
		width += px.width();
		height = std::max(height, px.height());
	}
	if (mTextLabel->isVisible()) {
		QFontMetrics fm(mTextLabel->font());
		QSize textSize = fm.size(Qt::TextSingleLine, mTextLabel->text());
		width += textSize.width();
		height = std::max(height, textSize.height());
	}
	width += mLayout->spacing();
	width = qMax(width, 100);
	return QSize(width, height);
}


QSize IdenticonLabel::sizeHint() const
{
	return minimumSizeHint();
}
