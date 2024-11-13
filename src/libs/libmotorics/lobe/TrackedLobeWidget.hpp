#ifndef TRACKEDLobeWIDGET_HPP
#define TRACKEDLobeWIDGET_HPP
#include "LobeWidget.hpp"

#include <QWidget>

namespace Ui
{
class TrackedLobeWidget;
}

class TrackedLobeWidget : public LobeWidget
{
	Q_OBJECT
private:
	Ui::TrackedLobeWidget *ui;

public:
	explicit TrackedLobeWidget(QWidget *parent = nullptr);
	~TrackedLobeWidget();
};

#endif
// TRACKEDLobeWIDGET_HPP


