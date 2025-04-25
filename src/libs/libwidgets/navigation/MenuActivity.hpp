#ifndef MENUACTIVITY_H
#define MENUACTIVITY_H

#include "components/navigation/Activity.hpp"
#include "uptime/ConfigureHelper.hpp"
#include "uptime/MethodGate.hpp"

#include <QMap>
#include <QPushButton>


class QPushButton;
class QVBoxLayout;


class MenuActivity : public Activity
{
	Q_OBJECT
private:
	QLayout *mLastGroupLayout{nullptr};
	QVBoxLayout *mVerticalLayoutGroups{nullptr};
	ConfigureHelper mConfigureHelper;
	
public:
	explicit MenuActivity(QWidget *parent = nullptr, const QString &name=QString());
	~MenuActivity();
	
private:
	QPushButton* createButton(const QString &title, const QString &iconPath = QString(":/icons/controller/unknown.svg"), const QString &toolTip=QString(), bool checkable=false, bool enabled=true);
public:
	
	void configure();
	QPushButton* addButton(const QString &title, const QString &iconPath = QString(":/icons/controller/unknown.svg"), const QString &toolTip=QString(), const QString &activityName = QString(), bool checkable=false, bool enabled=true);

	//template <typename MethodType>
	template <typename MethodType, typename = std::enable_if_t<!std::is_convertible<MethodType, QString>::value> >
	QPushButton* addButton(const QString &title, const QString &iconPath = QString(":/icons/controller/unknown.svg"), const QString &toolTip=QString(), MethodType onClick=[](){}, QObject *receiver = nullptr, bool checkable=false, bool enabled=true);
	
	void addSpacer();
	void addGroup(const QString &title);
/*
	// Virtual activity mechanism
protected:
	void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()) override;
	void pushImpl(const QStringList arguments = QStringList()) override;
*/
};


//template <typename MethodType>
template <typename MethodType, typename Enable>
QPushButton* MenuActivity::addButton(const QString &title, const QString &iconPath, const QString &toolTip, MethodType onClick, QObject *receiver, bool checkable, bool enabled){
	OC_METHODGATE();
	auto menuButton = createButton(title, iconPath, toolTip, checkable, enabled);
	if (receiver) {
		connect(menuButton, checkable ? &QPushButton::toggled : &QPushButton::clicked, receiver, onClick);
	} else {
		connect(menuButton, checkable ? &QPushButton::toggled : &QPushButton::clicked, onClick);
	}
	return menuButton;
}


#endif // MENUACTIVITY_H
