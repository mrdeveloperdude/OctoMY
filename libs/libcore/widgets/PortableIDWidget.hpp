#ifndef PORTABLEIDWIDGET_HPP
#define PORTABLEIDWIDGET_HPP

#include "security/PortableID.hpp"
#include <QWidget>

namespace Ui {
	class PortableIDWidget;
}

class PortableIDWidget : public QWidget
{
		Q_OBJECT
	private:
		Ui::PortableIDWidget *ui;
		PortableID mID;

	public:
		explicit PortableIDWidget(QWidget *parent = 0);
		virtual ~PortableIDWidget();

	public:

		void setPortableID(PortableID id);
		PortableID getPortableID();
};

#endif // PORTABLEIDWIDGET_HPP
