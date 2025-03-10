#include "TestSplashScreen.hpp"

#include "splash/SplashScreen.hpp"
#include "splash/PulsatingLabel.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

#include <QLabel>
#include <QPointer>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSequentialAnimationGroup>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

// Create a dummy window with some widgets inside
QWidget *createDummyWindow() {
	QWidget *dummy = OC_NEW QWidget();
	dummy->setWindowTitle("Dummy Test Window");
	dummy->resize(400, 300);
	
	// Create a layout and add some dummy widgets
	QVBoxLayout *layout = OC_NEW QVBoxLayout(dummy);
	QLabel *label1 = OC_NEW QLabel("Dummy Label 1", dummy);
	QLabel *label2 = OC_NEW QLabel("Dummy Label 2", dummy);
	QPushButton *dummyButton = OC_NEW QPushButton("Dummy Button", dummy);
	layout->addWidget(label1);
	layout->addWidget(label2);
	layout->addWidget(dummyButton);
	dummy->setLayout(layout);
	return dummy;
}

static QStringList dummyMessages;



// Create a floating control window with several buttons
QWidget *createControlWindow(SplashScreen *splash, QWidget *dummy) {
	// Use QPointer to ensure safety if splash is deleted
	QPointer<SplashScreen> safeSplash(splash);
	QWidget *controlWindow = OC_NEW QWidget();
	controlWindow->setWindowTitle("Control Panel");
	controlWindow->setFixedSize(200, 250);
	controlWindow->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
	
	QVBoxLayout *layout = OC_NEW QVBoxLayout(controlWindow);
	
	// (1) Checkable button: when toggled starts/stops a timer that resizes the dummy window in a circular motion
	QPushButton *resizeButton = OC_NEW QPushButton("Circular Resize", controlWindow);
	resizeButton->setCheckable(true);
	QTimer *resizeTimer = OC_NEW QTimer(controlWindow);
	static auto startSize = dummy->size();
	QObject::connect(resizeButton, &QPushButton::toggled, [=](bool checked) {
		if (checked){
			startSize = dummy->size();
			resizeTimer->start(1000 / 60);
		}
		else{
			resizeTimer->stop();
		}
	});
	
	QObject::connect(resizeTimer, &QTimer::timeout, [=]() {
		static qreal angle = 0.0;
		angle = fmod(angle + 0.05, M_PI * 2);
		// Circular motion: base size 400x300 plus offset from cosine and sine
		 
		dummy->resize(startSize.width() +  qCos(angle) * 100, startSize.height() + qSin(angle) * 100);
	});
	
	// (2) "Close Splash" button: calls done() on the splash screen
	QPushButton *doneButton = OC_NEW QPushButton("Close Splash", controlWindow);
	QObject::connect(doneButton, &QPushButton::clicked, [=]() {
		if (safeSplash)
			safeSplash->done();
	});
	
	// (3) "Random %" button: sets a random progress percentage
	QPushButton *randomProgressButton = OC_NEW QPushButton("Percentage = random%", controlWindow);
	QObject::connect(randomProgressButton, &QPushButton::clicked, [=]() {
		if (safeSplash) {
			int randomPercentage = QRandomGenerator::global()->bounded(1, 101);
			safeSplash->setProgressPercentage(randomPercentage);
		}
	});
	
	// (4) "Set 0%" button: resets progress to 0
	QPushButton *zeroProgressButton = OC_NEW QPushButton("Percentage = 0%", controlWindow);
	QObject::connect(zeroProgressButton, &QPushButton::clicked, [=]() {
		if (safeSplash)
			safeSplash->setProgressPercentage(0);
	});
	
	// (5) "Random Message" button: sets a random dummy message
	dummyMessages << "Loading data..." <<  "Initializing system..." << "Setting up UI..." << "Almost done!" << "Lorem Ipsum" << "Dolor Sit Amet" << "Scratching Deez Nutz";
	QPushButton *randomMessageButton = OC_NEW QPushButton("Random Message", controlWindow);
	QObject::connect(randomMessageButton, &QPushButton::clicked, [=]() {
		if (safeSplash) {
			static int index{0};
			index = (index + 1) % dummyMessages.size();
			safeSplash->setMessage(dummyMessages[index]);
		}
	});
	
	// (6) "Clear Message" button: clears the message
	QPushButton *noMessageButton = OC_NEW QPushButton("Clear Message", controlWindow);
	QObject::connect(noMessageButton, &QPushButton::clicked, [=]() {
		if (safeSplash)
			safeSplash->setMessage("");
	});
	
	// Add all buttons to the control window layout
	layout->addWidget(resizeButton);
	layout->addWidget(doneButton);
	layout->addWidget(randomProgressButton);
	layout->addWidget(zeroProgressButton);
	layout->addWidget(randomMessageButton);
	layout->addWidget(noMessageButton);
	controlWindow->setLayout(layout);
	return controlWindow;
}


// Creates a PulsatingLabel with the image loaded from imagePath.
// The label is animated to scale from 80% to 120% of its size and back, in a loop.
PulsatingLabel* createPulsatingLabel(const QString &imagePath, QWidget *parent = nullptr) {
	PulsatingLabel* label = new PulsatingLabel(parent);
	
	// Load the image
	QPixmap pixmap(imagePath);
	if (!pixmap.isNull()) {
		// Scale the image to a fixed size (300x300) while keeping aspect ratio
		//label->setPixmap(pixmap.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
		label->setSvg(imagePath);
	} else {
		qWarning() << "createPulsatingLabel: Could not load image:" << imagePath;
	}
	
	// Create a sequential animation group for pulsation
	auto group = new QSequentialAnimationGroup(label);
	
	// Forward animation: scale from 0.8 to 1.2
	QPropertyAnimation *forwardAnim = new QPropertyAnimation(label, "scaleFactor");
	forwardAnim->setDuration(500); // duration for scaling up
	forwardAnim->setStartValue(0.8);
	forwardAnim->setEndValue(1.0);
	forwardAnim->setEasingCurve(QEasingCurve::InOutQuad);
	
	// Reverse animation: scale back from 1.2 to 0.8
	QPropertyAnimation *backwardAnim = new QPropertyAnimation(label, "scaleFactor");
	backwardAnim->setDuration(500); // duration for scaling down
	backwardAnim->setStartValue(1.0);
	backwardAnim->setEndValue(0.8);
	backwardAnim->setEasingCurve(QEasingCurve::InOutQuad);
	
	// Add both animations to the group and loop indefinitely
	group->addAnimation(forwardAnim);
	group->addAnimation(backwardAnim);
	group->setLoopCount(-1);
	
	// Start the animation group
	group->start(QAbstractAnimation::DeleteWhenStopped);
	
	return label;
}



void TestSplashScreen::test() {
	Q_INIT_RESOURCE(icons);
	Q_INIT_RESOURCE(images);
	
	// Create dummy test window
	auto dummy = createDummyWindow();
	dummy->show();
	QString imagePath(":/icons/app/window/agent.svg");
	// Create and configure splash screen over the dummy window
	SplashScreen *splash = OC_NEW SplashScreen();
	splash->configure(dummy, imagePath);
	//splash->configure(dummy, ":/images/birth.jpeg");
	splash->setWindowTitle("Splash Screen Test");
	splash->setMinimumSize(300, 200);
	splash->setMessage("Loading...");
	splash->setProgressPercentage(30);
	
	// Create the control panel window with the interactive buttons
	QWidget *controlWindow = createControlWindow(splash, dummy);
	controlWindow->show();
	
	
	auto label = createPulsatingLabel(imagePath);
	label->show();
	
	// Wait for the UI to end (this blocks until the dummy window is closed)
	test::utility::waitForUIEnd(dummy);
}

OC_TEST_MAIN(test, TestSplashScreen)
