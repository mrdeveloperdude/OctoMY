#ifndef ACTIVITYSTACK_H
#define ACTIVITYSTACK_H

#include <QStackedWidget>

#include "components/navigation/Activity.hpp"

struct StackPage{
	Activity *widget{nullptr};
	bool headerEnabled{false};
	bool backEnabled{false};
	bool menuEnabled{false};
	
	explicit StackPage(Activity *widget=nullptr, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true)
		: widget(widget)
		, headerEnabled(headerEnabled)
		, backEnabled(backEnabled)
		, menuEnabled(menuEnabled)
	{
		
	}
};

class ActivityStack : public QWidget

{
	Q_OBJECT
private:
	Activity *mBottom{nullptr};
	QStackedWidget *mStackedWidget{nullptr};
	QMap<QString, StackPage> mPageMap;
	QStringList mStack;

public:
	explicit ActivityStack(QWidget *parent = nullptr);
	~ActivityStack();

public:
	void registerPage(Activity *page, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true);

public slots:
	QString pop(const QStringList returnArguments = QStringList());
	bool push(const QString &title, const QStringList arguments = QStringList());
	QString swap(const QString &title, const QStringList returnArguments = QStringList(), const QStringList arguments = QStringList());

signals:
	void done();
	void currentPageChanged(const StackPage &page);	
};

#endif // ACTIVITYSTACK_H
