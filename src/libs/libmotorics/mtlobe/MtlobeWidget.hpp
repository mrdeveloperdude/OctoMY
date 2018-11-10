#ifndef MTLOBEWIDGET_HPP
#define MTLOBEWIDGET_HPP

#include "utility/Standard.hpp"

#include <QWidget>



class MtlobeWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MtlobeWidget(QWidget *parent = nullptr);

signals:

	void mtlobeDeleted(quint32);
};

#endif // MTLOBEWIDGET_HPP
