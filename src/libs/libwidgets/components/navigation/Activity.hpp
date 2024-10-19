#ifndef ACTIVITY_HPP
#define ACTIVITY_HPP

#include <QWidget>


#include "uptime/ConfigureHelper.hpp"

class ActivityStack;


class Activity : public QWidget
{
	Q_OBJECT
private:
	friend class ActivityStack;
	ActivityStack *mStack{nullptr};
	bool mDebug{false};
	ConfigureHelper mConfigureHelperActivity;

public:
	explicit Activity(QWidget *parent = nullptr);

	// Internal activity mechanism
private:
	void setActivityStack(ActivityStack *stack);
	void popImpl_(const QString &returnActivity, const QStringList returnArguments = QStringList());
	void pushImpl_(const QStringList arguments = QStringList());
	
	// Virtual activity mechanism
protected:
	virtual void popImpl(const QString &returnActivity, const QStringList returnArguments = QStringList()){Q_UNUSED(returnArguments);}
	virtual void pushImpl(const QStringList arguments = QStringList()){Q_UNUSED(arguments);}
	
	// Convenience wrappers for activities to use stack
public:
	void pop(const QStringList returnArguments = QStringList());
	void push(const QString &activity, const QStringList arguments = QStringList());
	void swap(const QString &activity, const QStringList arguments = QStringList());

signals:
};

#endif // ACTIVITY_HPP
