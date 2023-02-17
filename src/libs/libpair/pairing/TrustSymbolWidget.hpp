#ifndef TRUSTSYMBOLWIDGET_HPP
#define TRUSTSYMBOLWIDGET_HPP

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
	void mouseDoubleClickEvent(QMouseEvent *) Q_DECL_OVERRIDE;
	void showEvent(QShowEvent *) Q_DECL_OVERRIDE;
	void hideEvent(QHideEvent *) Q_DECL_OVERRIDE;

private slots:
	void onPulsatingTrustTimer();

signals:
	void doubleClicked();

};


#endif
// TRUSTSYMBOLWIDGET_HPP
