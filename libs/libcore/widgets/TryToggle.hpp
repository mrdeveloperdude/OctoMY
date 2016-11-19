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


QString ToggleStateToSTring(TryToggleState s);

class TryToggle : public QWidget
{
	Q_OBJECT
private:
	QTimer mTimer;
	TryToggleState mState;
	QString mT1,mT2,mT3;

public:
	explicit TryToggle(QWidget *parent = 0);
	~TryToggle();

public:
	void setText(QString, QString, QString);
	TryToggleState state();

private:
	void updateText();


public slots:
	void setState(const TryToggleState s);


	void animateClick();
private slots:
	void onTimeout();

	void on_pushButtonToggle_toggled(bool checked);

private:
	Ui::TryToggle *ui;
signals:
	void stateChanged(const TryToggleState last, const TryToggleState current);
};


QDebug operator<<(QDebug d, const TryToggleState &s);


#endif // TRYTOGGLE_HPP
