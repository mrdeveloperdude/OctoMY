#include "MenuActivity.hpp"

#include "layout/FlowLayout.hpp"
#include "uptime/New.hpp"

#include <QLabel>
#include <QLayout>
#include <QSpacerItem>


MenuActivity::MenuActivity(QWidget *parent, const QString &name)
	: Activity(parent, name)
	, mVerticalLayoutGroups{new QVBoxLayout(this)}
	, mConfigureHelper("MenuActivity", true, false, false, true, false)
{
	OC_METHODGATE();
	mVerticalLayoutGroups->setObjectName("verticalLayoutGroups");
	mVerticalLayoutGroups->setContentsMargins(50, 50, 50, 50);
	setStyleSheet("QPushButton { text-align: left; padding-left: 10px; }");
}


MenuActivity::~MenuActivity()
{
	OC_METHODGATE();
}


QPushButton* MenuActivity::createButton(const QString &title, const QString &iconPath, const QString &toolTip, bool checkable, bool enabled){
	OC_METHODGATE();
	auto menuButton = OC_NEW QPushButton(this);
	menuButton->setObjectName(title);
	menuButton->setText(title);
	menuButton->setStatusTip(toolTip);
	menuButton->setMinimumSize(QSize(256, 128));
	if(!iconPath.isEmpty()){
		QIcon icon;
		icon.addFile(iconPath, QSize(), QIcon::Mode::Normal, QIcon::State::Off);
		menuButton->setIcon(icon);
		menuButton->setIconSize(QSize(96, 96));
	}
	//menuButton->setFlat(true);
	menuButton->setCheckable(checkable);
	menuButton->setEnabled(enabled);
	if(mLastGroupLayout){
		mLastGroupLayout->addWidget(menuButton);
	}
	else{
		mVerticalLayoutGroups->addWidget(menuButton);
	}
	return menuButton;
}


void MenuActivity::popImpl(const QString &returnActivity, const QStringList returnArguments)
{
	OC_METHODGATE();
	Q_UNUSED(returnActivity);
	Q_UNUSED(returnArguments);
	if(mConfigureHelper.isConfiguredAsExpected()) {
		
	}
}


void MenuActivity::pushImpl(const QStringList arguments)
{
	OC_METHODGATE();
	Q_UNUSED(arguments);
	qDebug() << "PUSH HUB MAIN MENU ACTIVITY: " << arguments;
	if(mConfigureHelper.isConfiguredAsExpected()) {

	}
}

void MenuActivity::addSpacer(){
	mVerticalLayoutGroups->addItem(OC_NEW QSpacerItem(30,30, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding) );
}

void MenuActivity::addGroup(const QString &title){
	auto flowLayout = OC_NEW FlowLayout();
	mLastGroupLayout = flowLayout;
	//groupLayout = OC_NEW QHBoxLayout();
	mLastGroupLayout->setObjectName("groupLayout" + title);
	auto labelGroupTitle = OC_NEW QLabel(this);
	labelGroupTitle->setObjectName("labelGroupTitle"+title);
	QFont font;
	font.setPointSize(16);
	font.setBold(true);
	labelGroupTitle->setFont(font);
	labelGroupTitle->setText(title);
	mVerticalLayoutGroups->addWidget(labelGroupTitle);
	mVerticalLayoutGroups->addLayout(mLastGroupLayout);
}


QPushButton* MenuActivity::addButton(const QString &title, const QString &iconPath, const QString &toolTip, const QString &activityName, bool checkable, bool enabled){
	return addButton(title, iconPath, toolTip, [activityName, this](){
		push(activityName);
	}, this, checkable, enabled);
}
