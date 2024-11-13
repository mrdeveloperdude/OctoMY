#ifndef HOVERINGLobeSWIDGET_HPP
#define HOVERINGLobeSWIDGET_HPP

#include "LobeWidget.hpp"

#include <QWidget>

namespace Ui
{
class HoveringLobeWidget;
}

class HoveringLobeWidget : public LobeWidget
{
	Q_OBJECT

public:
	explicit HoveringLobeWidget(QWidget *parent = nullptr);
	~HoveringLobeWidget();

private:
	Ui::HoveringLobeWidget *ui;
};

#endif
// HOVERINGLobeSWIDGET_HPP
