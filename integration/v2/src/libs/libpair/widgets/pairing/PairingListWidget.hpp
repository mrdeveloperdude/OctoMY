#ifndef PAIRINGLISTWIDGET_HPP
#define PAIRINGLISTWIDGET_HPP

#include <QWidget>

namespace Ui {
	class PairingListWidget;
	}

class PairingListWidget : public QWidget
{
		Q_OBJECT

	public:
		explicit PairingListWidget(QWidget *parent = nullptr);
		~PairingListWidget();

	private:
		Ui::PairingListWidget *ui;
};

#endif // PAIRINGLISTWIDGET_HPP
