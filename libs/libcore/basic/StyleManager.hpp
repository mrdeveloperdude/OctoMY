
#ifndef STYLEMANAGER_HPP
#define STYLEMANAGER_HPP

#include <QString>
#include <QApplication>
#include <QColor>

class StyleManager{
	private:
		QColor tinge;
	public:
		StyleManager(QColor tinge=QColor(142,45,197).lighter());
		void apply();
	private:
		bool loadFont(QString name);
		void loadFonts(QApplication &app);
		void applyStyle(QApplication &app);
		void applyStyleSheet(QApplication &app);

};

#endif // STYLEMANAGER_HPP
