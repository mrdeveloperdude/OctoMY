#ifndef ACTIVITYSTACK_HPP
#define ACTIVITYSTACK_HPP

#include <QWidget>
#include <QMap>
#include <QStringList>

#include "components/navigation/Activity.hpp"
#include "uptime/SharedPointerWrapper.hpp"


struct StackPage {
	QSharedPointer<Activity> widget;
	bool headerEnabled{false};
	bool backEnabled{false};
	bool menuEnabled{false};
	
	explicit StackPage(QSharedPointer<Activity> widget = nullptr,
		bool headerEnabled = true,
		bool backEnabled = true,
		bool menuEnabled = true)
		: widget(widget)
		, headerEnabled(headerEnabled)
		, backEnabled(backEnabled)
		, menuEnabled(menuEnabled)
	{}
};

class ActivityStack : public QWidget {
	Q_OBJECT
private:
	QSharedPointer<Activity> mBottom{nullptr};
	QMap<QString, StackPage> mActivityMap;
	QStringList mStack;

public:
	explicit ActivityStack(QWidget *parent = nullptr);
	~ActivityStack();
	
	void registerActivity(QSharedPointer<Activity> activity, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true);
	
public slots:
	QString pop(const QStringList returnArguments = QStringList());
	bool push(const QString &title, const QStringList arguments = QStringList());
	QString swap(const QString &title, const QStringList returnArguments = QStringList(), const QStringList arguments = QStringList());
	
signals:
	void done();
	void currentPageChanged(const StackPage &activity);
	
protected:
	// When the ActivityStack is resized, adjust the geometry of the currently visible activity.
	void resizeEvent(QResizeEvent *event) override;
};

#endif // ACTIVITYSTACK_HPP
