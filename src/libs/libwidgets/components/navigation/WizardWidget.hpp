#ifndef WIZARDWIDGET_H
#define WIZARDWIDGET_H

#include <QWidget>

namespace Ui {
class WizardWidget;
}



struct WizardPage{
	QWidget *widget{nullptr};
	bool headerEnabled{false};
	bool backEnabled{false};
	bool menuEnabled{false};

	explicit WizardPage(QWidget *widget=nullptr, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true)
		: widget(widget)
		, headerEnabled(headerEnabled)
		, backEnabled(backEnabled)
		, menuEnabled(menuEnabled)
	{
		
	}
};


class WizardWidget : public QWidget
{
	Q_OBJECT
private:
	Ui::WizardWidget *ui;
	bool mLooping{false};
	QMap<QString, WizardPage> mPageMap;

public:
	explicit WizardWidget(QWidget *parent = nullptr);
	~WizardWidget();

private:
	void updateUI();

public:
	void registerPage(QWidget *page, bool headerEnabled = true, bool backEnabled = true, bool menuEnabled = true);

public slots:
	void reset();
	void navigate(const QString &title);
	void navigateRelative(int rel);
	void navigateAbsolute(int target);
	void prev();
	void next();

signals:
	void done();
	void currentPageChanged(const WizardPage &page);	
};

#endif // WIZARDWIDGET_H
