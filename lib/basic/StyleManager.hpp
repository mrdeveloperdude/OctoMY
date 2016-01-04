
#ifndef STYLEMANAGER_HPP
#define STYLEMANAGER_HPP

#include <QString>
#include <QApplication>

class StyleManager{
	public:
		StyleManager();
		void apply();
	private:
		bool loadFont(QString name);
		void loadFonts(QApplication &app);
		void applyStyle(QApplication &app);
		void applyStyleSheet(QApplication &app);

};

#endif // STYLEMANAGER_HPP
