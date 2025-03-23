#include "ScreenManager.hpp"

#include "app/Settings.hpp"
#include "screen/ScreenSelector.hpp"
#include "uptime/MethodGate.hpp"
#include "uptime/New.hpp"

#include <QGuiApplication>
#include <algorithm>

ScreenManager* ScreenManager::instance() {
	static ScreenManager* s_instance = new ScreenManager();
	return s_instance;
}

ScreenManager::ScreenManager(QObject *parent)
	: QObject(parent)
	, mConfigureHelper("ScreenManager", true, false, false)
{
	auto guiApp = static_cast<QGuiApplication*>(QGuiApplication::instance());
	for (auto &screen : QGuiApplication::screens()) {
		mScreens.append(screen);
	}
	connect(guiApp, &QGuiApplication::screenAdded, this, [this](QScreen *screen) {
		mScreens.append(screen);
		emit screensChanged();
		updateWindowPositions();
	});
	connect(guiApp, &QGuiApplication::screenRemoved, this, [this](QScreen *screen) {
		mScreens.removeAll(screen);
		emit screensChanged();
		updateWindowPositions();
	});
	for (auto &screen : mScreens) {
		connect(screen, &QScreen::geometryChanged, this, [this]() {
			emit screensChanged();
			updateWindowPositions();
		});
	}
}


void ScreenManager::configure(QSharedPointer<Settings> s)
{
	OC_METHODGATE();
	if(mConfigureHelper.configure()) {
		if(mSettings != s) {
			mSettings = s;
		}
	}
}

QList<QScreen*> ScreenManager::availableScreens() const {
	QList<QScreen*> list;
	for (auto &screen : mScreens){
		if (screen){
			list.append(screen);
		}
	}
	return list;
}

QScreen* ScreenManager::screenAt(const QPoint &point) const {
	for (auto screen : availableScreens()) {
		if (screen->geometry().contains(point)){
			return screen;
		}
	}
	return QGuiApplication::primaryScreen();
}

QRect ScreenManager::findWindowPlacement(const QSize &windowSize) const {
	QScreen *primary = QGuiApplication::primaryScreen();
	if (!primary)
		return QRect(QPoint(0, 0), windowSize);
	
	QRect avail = primary->availableGeometry();
	int x = avail.x() + (avail.width() - windowSize.width()) / 2;
	int y = avail.y() + (avail.height() - windowSize.height()) / 2;
	return QRect(QPoint(x, y), windowSize);
}


QRect ScreenManager::calculateOptimalGeometry(const WindowAttributes &attributes) {
	QList<QScreen*> screens = availableScreens();
	QScreen *primaryScreen = nullptr;
	QScreen *toolScreen = nullptr;
	
	if (screens.size() >= 3) {
		std::sort(screens.begin(), screens.end(), [](QScreen* a, QScreen* b){
			return a->geometry().x() < b->geometry().x();
		});
		primaryScreen = screens.at(1); // Middle screen.
		toolScreen = screens.at(2);    // Right-most screen.
	} else if (screens.size() == 2) {
		QScreen *screen1 = screens.at(0);
		QScreen *screen2 = screens.at(1);
		if (screen1->geometry().width() * screen1->geometry().height() >= 
			screen2->geometry().width() * screen2->geometry().height()) {
			primaryScreen = screen1;
			toolScreen = screen2;
		} else {
			primaryScreen = screen2;
			toolScreen = screen1;
		}
	} else if (screens.size() == 1) {
		primaryScreen = screens.at(0);
		toolScreen = screens.at(0);
	} else {
		return QRect(0, 0, attributes.preferredSize.width(), attributes.preferredSize.height());
	}
	
	QRect suggestedGeometry;
	if (attributes.type == WindowType::Primary) {
		suggestedGeometry = primaryScreen->availableGeometry();
	} else if (attributes.type == WindowType::Tool) {
		QRect avail = toolScreen->availableGeometry();
		suggestedGeometry = QRect(avail.topLeft(), attributes.preferredSize);
	} else {
		QRect avail = primaryScreen->availableGeometry();
		int x = avail.x() + (avail.width() - attributes.preferredSize.width()) / 2;
		int y = avail.y() + (avail.height() - attributes.preferredSize.height()) / 2;
		suggestedGeometry = QRect(QPoint(x, y), attributes.preferredSize);
	}
	return suggestedGeometry;
}

QRect ScreenManager::restore(const QString &windowKey, const WindowAttributes &attributes) {
	mWindowKeys.insert(windowKey);
	auto storedGeometry = restoreStorage(windowKey);
	if (storedGeometry.isValid() && storedGeometry.width() > 0 && storedGeometry.height() > 0){
		return storedGeometry;
	}
	else{
		auto suggestedGeometry=calculateOptimalGeometry(attributes);
		saveStorage(windowKey, suggestedGeometry);
		return suggestedGeometry;
	}
}

void ScreenManager::save(const QString &windowKey, const QRect &geometry) {
	mWindowKeys.insert(windowKey);
	saveStorage(windowKey, geometry);
}

void ScreenManager::updateWindowPositions() {
	if (!mSettings){
		return;
	}
	for (const QString &windowKey : mWindowKeys) {
		QRect oldGeom = restoreStorage(windowKey);
		WindowAttributes attr { WindowType::Primary, oldGeom.size() };
		QRect newGeom = restore(windowKey, attr);
		saveStorage(windowKey, newGeom);
	}
}



void ScreenManager::selectScreen(){
	
	for(auto &screen:mScreens){
		auto ss = OC_NEW ScreenSelector();
		ss->setScreen(screen);
		connect(ss, &ScreenSelector::screenSelected, this, &ScreenManager::screenSelected);
		mScreenSelectors << ss;
	}
	for(auto &selector:mScreenSelectors){
		selector->show();
	}
}

void ScreenManager::screenSelected(QScreen *screen){
	Q_UNUSED(screen);
	for(auto &selector:mScreenSelectors){
		selector->hide();
		selector->deleteLater();
	}
	mScreenSelectors.clear();
}

void ScreenManager::saveStorage(const QString &key, const QRect &geometry) {
	if(mSettings){
		mSettings->setCustomSettingRect("window-geometry-" + key, geometry);
	}
}

QRect ScreenManager::restoreStorage(const QString &key) {
	if(mSettings){
		return mSettings->getCustomSettingRect("window-geometry-" + key);
	}
	return QRect();
}
