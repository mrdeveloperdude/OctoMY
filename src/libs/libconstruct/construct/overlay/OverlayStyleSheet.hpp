#ifndef OVERLAYSTYLE_SHEET_HPP
#define OVERLAYSTYLE_SHEET_HPP

#include "OverlayStates.hpp"

#include <QColor>
#include <QHash>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// Structure holding individual overlay style parameters.
struct OverlayStyle {
	QColor tintColor;         // The tint color applied to the node's surfaces.
	float tintOpacity;        // Tint opacity (0.0 to 1.0).
	bool showOutline;         // Whether to draw an outline.
	QColor outlineColor;      // Outline color.
	float outlineThickness;   // Outline thickness (in pixels).
	bool showHandles;         // Whether to render editing handles.
	QColor handleColor;       // Color for handles.
	float handleSize;         // Size for handles (in pixels).
	
	OverlayStyle();
	OverlayStyle(const QColor &tint, float opacity,
				 bool outline, const QColor &oColor, float oThickness,
				 bool handles = false, const QColor &hColor = QColor(), float hSize = 0.0f);
};

// The style sheet holds a mapping from state combinations to overlay styles.
class OverlayStyleSheet {
public:
	OverlayStyleSheet();
	
	// Adds (or overrides) a style for a given combination of states.
	void addStyle(OverlayStates states, const OverlayStyle &style);
	
	// Resolves and returns the effective style for a given set of active states.
	OverlayStyle resolveStyle(OverlayStates states) const;
	
	// Optionally, load styles from a JSON file.
	bool loadFromFile(const QString &filePath);
	
private:
	QHash<OverlayStates, OverlayStyle> m_styles;
	OverlayStyle m_defaultStyle;
};

#endif // OVERLAYSTYLE_SHEET_HPP
