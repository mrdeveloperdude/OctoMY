#ifndef TESTWIDGET_HPP
#define TESTWIDGET_HPP

#include <QWidget>

namespace Ui
{
class TestWidget;
}


/**
 * @brief The TestWidget class is a dummy widget used for testing.
 */
class TestWidget : public QWidget
{
	Q_OBJECT

public:
	explicit TestWidget(QWidget *parent = nullptr);
	~TestWidget();

private:
	Ui::TestWidget *ui;
};

#endif
// TESTWIDGET_HPP
