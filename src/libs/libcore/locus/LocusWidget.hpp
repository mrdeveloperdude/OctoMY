#ifndef LOCUSWIDGET_HPP
#define LOCUSWIDGET_HPP

#include <QWidget>

class LocusWidget : public QWidget
{
	Q_OBJECT
public:
	explicit LocusWidget(QWidget *parent = 0);

signals:

	void locusDeleted();
};

#endif // LOCUSWIDGET_HPP
