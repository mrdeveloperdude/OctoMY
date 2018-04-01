#ifndef CLIENTWIDGET_HPP
#define CLIENTWIDGET_HPP

#include <QWidget>

class ClientWidget : public QWidget
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
