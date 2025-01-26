#ifndef TRIPARAMETERWIDGET_HPP
#define TRIPARAMETERWIDGET_HPP

#include <QWidget>
#include <QSharedPointer>

class Tri;

namespace Ui {
class TriParameterWidget;
}

class TriParameterWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::TriParameterWidget *ui;
	QSharedPointer<Tri> mTri;

public:
	explicit TriParameterWidget(QWidget *parent = nullptr);
	~TriParameterWidget();
public:
	
	
	void setTri(QSharedPointer<Tri> tri);
	
};

#endif // TRIPARAMETERWIDGET_HPP
