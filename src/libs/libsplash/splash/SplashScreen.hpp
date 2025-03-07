#ifndef SPLASHSCREEN_HPP
#define SPLASHSCREEN_HPP

#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QGraphicsOpacityEffect>

namespace Ui {
class SplashScreen;
}

class SplashScreen : public QWidget {
	Q_OBJECT
private:
	Ui::SplashScreen *ui;
	QAbstractAnimation *pulseAnimation;
	quint8 progressPercentage;
	QGraphicsOpacityEffect *opacityEffect;
	QPropertyAnimation *fadeOut;
	QPropertyAnimation *forwardAnim;
	QPropertyAnimation *backwardAnim;

protected:
	bool eventFilter(QObject *watched, QEvent *event) override;
	
public:
	explicit SplashScreen(QWidget *parent = nullptr);
	~SplashScreen();
	
	// Configure the splash screen; note the fadeTime is in milliseconds
	void configure(QWidget *parent, const QString &imagePath, const quint16 pulseTime=500, const quint16 fadeTime=500, const qreal lowSize=0.90);
	
public slots:
	void setProgressPercentage(quint8 percentage);
	void setMessage(const QString &message);
	void done();
	
signals:
	void progressUpdated(quint8 percentage);
};

#endif // SPLASHSCREEN_HPP
