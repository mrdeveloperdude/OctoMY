#ifndef MTLOBEWIDGET_HPP
#define MTLOBEWIDGET_HPP

#include <QWidget>

class MtlobeWidget : public QWidget
{
	Q_OBJECT
public:
	explicit MtlobeWidget(QWidget *parent = 0);

signals:

	void mtlobeDeleted(quint32);
};

#endif // MTLOBEWIDGET_HPP
