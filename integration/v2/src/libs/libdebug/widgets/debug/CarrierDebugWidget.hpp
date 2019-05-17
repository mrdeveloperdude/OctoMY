#ifndef CARRIERDEBUGWIDGET_HPP
#define CARRIERDEBUGWIDGET_HPP

#include <QWidget>

namespace Ui {
	class CarrierDebugWidget;
	}

class CarrierDebugWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit CarrierDebugWidget(QWidget *parent = nullptr);
		~CarrierDebugWidget();

	private:
		Ui::CarrierDebugWidget *ui;
};

#endif // CARRIERDEBUGWIDGET_HPP
