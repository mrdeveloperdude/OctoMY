#include "ActivityStack.hpp"

#include <QResizeEvent>
#include <QVBoxLayout>
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

ActivityStack::ActivityStack(QWidget *parent)
	: QWidget(parent)
	, mBottom{QSharedPointer<Activity>::create()}
{
	OC_METHODGATE();
	// Create the bottom (default) activity.
	mBottom->setObjectName("ActivityStackBottomPage");
	registerActivity(mBottom);
	mStack.push_back(mBottom->objectName());
	// Make the bottom activity visible and fill the entire area.
	mBottom->setVisible(true);
	mBottom->setGeometry(rect());
}

ActivityStack::~ActivityStack()
{
	OC_METHODGATE();
	mBottom.clear();
	mActivityMap.clear();
	mStack.clear();
}

void ActivityStack::registerActivity(QSharedPointer<Activity> activity, bool headerEnabled, bool backEnabled, bool menuEnabled)
{
	OC_METHODGATE();
	auto title = activity->activityName();
	mActivityMap[title] = StackPage(activity, headerEnabled, backEnabled, menuEnabled);
	activity->setActivityStack(this);
	// Parent the activity to this widget.
	activity->setParent(this);
	// If this activity is the current one, make it visible and set its geometry; otherwise, hide it.
	if (mStack.isEmpty() || mStack.last() == title) {
		activity->setVisible(true);
		activity->setGeometry(rect());
	} else {
		activity->setVisible(false);
	}
}

bool ActivityStack::push(const QString &title, const QStringList arguments)
{
	OC_METHODGATE();
	if (mActivityMap.contains(title)) {
		auto activityEntry = mActivityMap[title];
		if (activityEntry.widget != nullptr) {
			// Disable updates on the currently visible activity.
			if (!mStack.isEmpty()) {
				auto currentTitle = mStack.last();
				if (mActivityMap.contains(currentTitle)) {
					auto currentActivity = mActivityMap[currentTitle].widget;
					if (currentActivity){
						currentActivity->setUpdatesEnabled(false);
					}
				}
			}
			// Push the new activity's key onto the stack.
			mStack.push_back(title);
			// Parent the new activity to this widget, set its geometry and make it visible.
			activityEntry.widget->setParent(this);
			activityEntry.widget->setGeometry(rect());
			activityEntry.widget->setVisible(true);
			activityEntry.widget->setUpdatesEnabled(true);
			// Hide the previous activity.
			if (mStack.size() > 1) {
				QString previousTitle = mStack.at(mStack.size() - 2);
				if (mActivityMap.contains(previousTitle)) {
					auto previousActivity = mActivityMap[previousTitle].widget;
					if (previousActivity){
						previousActivity->setUpdatesEnabled(false);
						previousActivity->setVisible(false);
						previousActivity->setParent(nullptr);
					}
				}
			}
			activityEntry.widget->pushImpl_(arguments);
			return true;
		} else {
			qWarning() << "activityEntry.widget was null";
		}
	} else {
		qWarning() << "no activity named " << title << " registered";
	}
	return false;
}

QString ActivityStack::pop(const QStringList returnArguments)
{
	OC_METHODGATE();
	if (mStack.size() > 1) {
		auto back = mStack.back();
		mStack.pop_back();
		auto title = mStack.back();
		if (mActivityMap.contains(title)) {
			auto activityEntry = mActivityMap[title];
			// Disable updates on the current activity before hiding it.
			if (mActivityMap.contains(back)) {
				auto currentActivity = mActivityMap[back].widget;
				if (currentActivity) {
					currentActivity->setUpdatesEnabled(false);
					currentActivity->setVisible(false);
					currentActivity->setParent(nullptr);
				}
			}
			// Show the previous activity.
			if (activityEntry.widget) {
				activityEntry.widget->setParent(this);
				activityEntry.widget->setGeometry(rect());
				activityEntry.widget->setVisible(true);
				activityEntry.widget->setUpdatesEnabled(true);
				activityEntry.widget->popImpl_(back, returnArguments);
				return back;
			}
		} else {
			// Failed: push the popped key back onto the stack.
			mStack.push_back(back);
		}
	} else {
		qWarning() << "stack empty trying to pop";
	}
	return "";
}

QString ActivityStack::swap(const QString &title,
	const QStringList returnArguments,
	const QStringList arguments)
{
	OC_METHODGATE();
	QString back = pop(returnArguments);
	if (back != "") {
		if (push(title, arguments)) {
			return back;
		}
	}
	return "";
}

void ActivityStack::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	// When the ActivityStack is resized, ensure that the currently visible activity fills it.
	if (!mStack.isEmpty()) {
		auto currentTitle = mStack.last();
		if (mActivityMap.contains(currentTitle)) {
			auto activity = mActivityMap[currentTitle].widget;
			if (activity) {
				activity->setGeometry(rect());
			}
		}
	}
}
