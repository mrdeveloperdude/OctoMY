#ifndef SCREENSELECTOR_HPP
#define SCREENSELECTOR_HPP

#include <QWidget>

class QScreen;

namespace Ui {
class ScreenSelector;
}

class ScreenSelector : public QWidget
{
	Q_OBJECT
private:
	Ui::ScreenSelector *ui;
	QScreen *mScreen{nullptr};

public:
	explicit ScreenSelector(QWidget *parent = nullptr);
	~ScreenSelector();

public:
	void setScreen(QScreen *screen);
private slots:
	void selectScreen();
	
signals:
	void screenSelected(QScreen *screen);
};

#endif // SCREENSELECTOR_HPP
