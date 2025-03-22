#ifndef SCREENMANAGER_HPP
#define SCREENMANAGER_HPP

#include "uptime/ConfigureHelper.hpp"
#include "uptime/SharedPointerWrapper.hpp"

#include <QHash>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QString>
#include <QSet>

class Settings;
class ScreenSelector;

// Define the type of window we’re placing.
enum class WindowType {
	Primary,
	Tool,
	Other
};

// Attributes that the caller passes in when restoring a window.
struct WindowAttributes {
	WindowType type;
	QSize preferredSize;
};

class ScreenManager : public QObject {
	Q_OBJECT
private:
	// Our current set of screens.
	QList<QPointer<QScreen>> mScreens;
	// Instead of using a QHash, we now rely on persistent storage via Settings,
	// and maintain our own cache of keys.
	QSet<QString> mWindowKeys;
	QSharedPointer<Settings> mSettings;
	QVector<ScreenSelector *> mScreenSelectors;
	ConfigureHelper mConfigureHelper;
public:
	explicit ScreenManager(QObject *parent = nullptr);
	static ScreenManager* instance();
	
private:
	void updateScreens();
	void saveStorage(const QString &key, const QRect &geometry);
	QRect restoreStorage(const QString &key);
	
public:
	void configure(QSharedPointer<Settings> settings);
	
	// Basic screen queries.
	QList<QScreen*> availableScreens() const;
	QScreen* screenAt(const QPoint &point) const;
	QRect findWindowPlacement(const QSize &windowSize) const;
	
	QRect calculateOptimalGeometry(const WindowAttributes &attributes);
	// Save/restore window geometry by key.
	// If no saved geometry exists for windowKey, a smart suggestion is returned based on attributes.
	QRect restore(const QString &windowKey, const WindowAttributes &attributes);
	void save(const QString &windowKey, const QRect &geometry);
	
	// When screens change (or are reconfigured), update stored window positions.
	void updateWindowPositions();
	
	void selectScreen();
	
private slots:
	void screenSelected(QScreen *screen);
	
signals:
	void screensChanged(); // Emitted when screens are added, removed, or reconfigured.
	
};

#endif // SCREENMANAGER_HPP
