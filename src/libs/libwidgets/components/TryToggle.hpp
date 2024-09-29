#ifndef TRYTOGGLE_HPP
#define TRYTOGGLE_HPP

#include "TryToggleState.hpp"

#include <QWidget>

#include <QTimer>
#include <QDebug>


namespace Ui
{
class TryToggle;
}


class TryToggle : public QWidget
{
	Q_OBJECT
private:
	Ui::TryToggle *ui;
	QTimer mTimer;
	TryToggleState mState;
	QString mOffText;
	QString	mGoingOnText;
	QString mOnText;
	QString mGoingOffText;
	QPalette mOnPalette;
	QPalette mOffPalette;
	QPalette mGoingOffPalette;
	bool mTimerToggle;

public:
	explicit TryToggle(QWidget *parent = nullptr);
	virtual ~TryToggle();

public:
	void configure(const QString offText, const QString goingOnText, const QString onText, const QString goingOffText, const QColor onColor=QColor(), const QColor goingOffColor=QColor(), const quint32 blinkInterval=1000/10);
	void setColor(const QColor onColor, const QColor goingOffColor=QColor());

	TryToggleState state() const;
	bool isPositive();
	bool isTransitive();

private:
	void updateText();

public slots:
	void setState(const TryToggleState s, const bool doEmit=true);
	void animateClick();

private slots:
	void onTimeout();
	void onToggled(bool checked);

signals:
	void stateChanged(const TryToggleState last, const TryToggleState current);
};


#endif
// TRYTOGGLE_HPP
