#ifndef PAIRINGLISTWIDGET_HPP
#define PAIRINGLISTWIDGET_HPP

#include <QWidget>

namespace Ui
{
class PairingListWidget;
}

class PairingListWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::PairingListWidget *ui;

public:
	explicit PairingListWidget(QWidget *parent = nullptr);
	~PairingListWidget();

};

#endif
// PAIRINGLISTWIDGET_HPP
