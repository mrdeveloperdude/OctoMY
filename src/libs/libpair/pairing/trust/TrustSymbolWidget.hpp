#ifndef TRUSTSYMBOLWIDGET_H
#define TRUSTSYMBOLWIDGET_H

#include "components/SvgWidget.hpp"
#include "uptime/ConfigureHelper.hpp"

#include "TrustLevel.hpp"

#include <QTimer>

class TrustSymbolWidget : public SvgWidget
{
	Q_OBJECT
private:
	QTimer mPulsatingTrustTimer;
	TrustLevel mLevel;
	ConfigureHelper mConfigureHelper;

	static const QRgb palette[];
	static const int paletteSz;

public:
	explicit TrustSymbolWidget(QWidget *parent = nullptr);

public:
	void configure();
	void setTrustLevel(TrustLevel level);
	void updatePulsating();

protected:
	void mouseDoubleClickEvent(QMouseEvent *) override;
	void showEvent(QShowEvent *) override;
	void hideEvent(QHideEvent *) override;

private slots:
	void onPulsatingTrustTimer();

signals:
	void doubleClicked();

};


#endif
// TRUSTSYMBOLWIDGET_HPP
