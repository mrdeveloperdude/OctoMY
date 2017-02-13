#ifndef NAMEMAPPINGVIEW_HPP
#define NAMEMAPPINGVIEW_HPP

#include <QWidget>

class NameMappingWidget;

class NameMappingView: public QWidget
{
	Q_OBJECT

private:
	NameMappingWidget *mWidget;

public:
	explicit NameMappingView(QWidget *parent = nullptr);

public:
	void configure(NameMappingWidget &mapping);
protected:

	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


};

#endif // NAMEMAPPINGVIEW_HPP
