#ifndef POSEMAPPINGVIEW_HPP
#define POSEMAPPINGVIEW_HPP

#include <QWidget>

class PoseMappingWidget;

class PoseMappingView : public QWidget
{
	Q_OBJECT
private:
	PoseMappingWidget *mWidget;

public:
	explicit PoseMappingView(QWidget *parent = nullptr);

public:
	void configure(PoseMappingWidget &mapping);

protected:
	void paintEvent(QPaintEvent *event) override;
};

#endif
// POSEMAPPINGVIEW_HPP
