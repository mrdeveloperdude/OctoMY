#ifndef IRISWIDGET_HPP
#define IRISWIDGET_HPP


#include "expression/IrisRendrer.hpp"
#include "identity/Identicon.hpp"
#include "security/PortableID.hpp"

#include <QDomDocument>
#include <QWidget>
#include <QPixmap>


class IrisWidget: public QWidget
{
	Q_OBJECT

private:
	Identicon identicon;
	QPixmap mDoubleBuffer;
	bool mDirty;
	PortableID mPid;
	quint32 mIndex;

public:
	explicit IrisWidget(QWidget *parent = 0);

protected:
	void regenerateWidget();

public:
	void setIrixIndex(quint32 index);
	void setPortableID(PortableID &id);

	PortableID portableID();

signals:
	void doubleClicked();

	// Widget events
protected:
	void mouseDoubleClickEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

};



#endif // IRISWIDGET_HPP
