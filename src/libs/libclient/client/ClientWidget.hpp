#ifndef CLIENTWIDGET_HPP
#define CLIENTWIDGET_HPP

#include "components/navigation/Activity.hpp"

#include <QWidget>

class ClientWidget : public Activity
{
	Q_OBJECT
public:
	explicit ClientWidget(QWidget *parent = nullptr);
	virtual ~ClientWidget();

	// ClientWidget interface
public:
	virtual void updateControlLevel(int level) =0;

};

#endif // CLIENTWIDGET_HPP
