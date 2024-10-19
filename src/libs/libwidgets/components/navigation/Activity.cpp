#include "Activity.hpp"
#include "uptime/MethodGate.hpp"

#include "ActivityStack.hpp"

Activity::Activity(QWidget *parent)
	: QWidget{parent}
	, mConfigureHelperActivity("Activity", true, false, false, true, false)
{
	OC_METHODGATE();
}


void Activity::setActivityStack(ActivityStack *stack){
	OC_METHODGATE();
	if(mConfigureHelperActivity.configure()) {
		if(mDebug){
			qDebug() << objectName() << ": setActivityStack()";
		}
		mStack = stack;
	}
}


void Activity::popImpl_(const QString &returnActivity, const QStringList returnArguments){
	OC_METHODGATE();
	if(mConfigureHelperActivity.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug() << objectName() << ": popImpl()";
		}
		popImpl(returnActivity, returnArguments);
	}
}


void Activity::pushImpl_(const QStringList arguments){
	OC_METHODGATE();
	if(mConfigureHelperActivity.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug() << objectName() << ": pushImpl(" << arguments.join(", ") << ")";
		}
		pushImpl(arguments);
	}
}


void Activity::pop(const QStringList returnArguments){
	OC_METHODGATE();
	if(mConfigureHelperActivity.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug() << objectName() << ": pop()";
		}
		if(nullptr != mStack){
			mStack->pop(returnArguments);
		}
		else{
			qWarning()<<"No stack for pop";
		}
	}
}


void Activity::push(const QString &activity, const QStringList arguments){
	OC_METHODGATE();
	if(mConfigureHelperActivity.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug() << objectName() << ": push(" << activity << ", " << arguments.join(", ") << ")";
		}
		if(nullptr != mStack){
			mStack->push(activity, arguments);
		}
		else{
			qWarning()<<"No stack for push";
		}
	}
}


void Activity::swap(const QString &activity, const QStringList arguments){
	OC_METHODGATE();
	if(mConfigureHelperActivity.isConfiguredAsExpected()) {
		if(mDebug){
			qDebug() << objectName() << ": swap(" << activity << ", " << arguments.join(", ") << ")";
		}
		if(nullptr != mStack){
			mStack->swap(activity, arguments);
		}
		else{
			qWarning()<<"No stack for swap";
		}
	}
}
