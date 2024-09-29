#ifndef PAGEREGISTRY_HPP
#define PAGEREGISTRY_HPP

#include <QString>
#include <QMap>

class QWidget;





struct RegisteredPage{
	QWidget *widget{nullptr};
	bool headerEnabled{false};
	bool backEnabled{false};
	bool menuEnabled{false};
	explicit RegisteredPage(QWidget *widget = nullptr, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true)
		: widget(widget)
		, headerEnabled(headerEnabled)
		, backEnabled(backEnabled)
		, menuEnabled(menuEnabled)
	{
		
	}
};

class PageRegistry
{
private:
	QMap<QString, RegisteredPage> mPageMap;

public:
	PageRegistry();
	
	
public:
	void registerPage(QWidget *page, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true);
	
	
	RegisteredPage borrow(const QString &name);
};

#endif // PAGEREGISTRY_HPP
