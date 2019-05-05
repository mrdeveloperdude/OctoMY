#ifndef COMMSDEBUGWIDGET_HPP
#define COMMSDEBUGWIDGET_HPP

#include <QWidget>

namespace Ui {
	class CommsDebugWidget;
	}

class CommsDebugWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit CommsDebugWidget(QWidget *parent = nullptr);
		~CommsDebugWidget();

	private:
		Ui::CommsDebugWidget *ui;
};

#endif // COMMSDEBUGWIDGET_HPP
