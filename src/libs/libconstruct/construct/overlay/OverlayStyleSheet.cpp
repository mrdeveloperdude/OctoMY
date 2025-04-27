#include "OverlayStyleSheet.hpp"
#include <QFile>

// Default constructor for OverlayStyle.
OverlayStyle::OverlayStyle()
	: tintColor(Qt::transparent)
	, tintOpacity(0.0f)
	, showOutline(false)
	, outlineColor(Qt::black)
	, outlineThickness(0.0f)
	, showHandles(false)
	, handleColor(Qt::black)
	, handleSize(0.0f)
{
}

// Parameterized constructor.
OverlayStyle::OverlayStyle(const QColor &tint, float opacity,
	bool outline, const QColor &oColor, float oThickness,
	bool handles, const QColor &hColor, float hSize)
	: tintColor(tint)
	, tintOpacity(opacity)
	, showOutline(outline)
	, outlineColor(oColor)
	, outlineThickness(oThickness)
	, showHandles(handles)
	, handleColor(hColor)
	, handleSize(hSize)
{
}

// Default style sheet constructor: set a sensible default style.
OverlayStyleSheet::OverlayStyleSheet()
	: m_defaultStyle(OverlayStyle(Qt::transparent, 0.0f, false, Qt::black, 0.0f))
{
}

// Add a style for a given state combination.
void OverlayStyleSheet::addStyle(OverlayStates states, const OverlayStyle &style)
{
	m_styles.insert(states, style);
}

// Resolve the effective style for the given active states.
// This simple implementation first checks for an exact match,
// then looks for individual state overrides.
OverlayStyle OverlayStyleSheet::resolveStyle(OverlayStates states) const
{
	if (m_styles.contains(states))
		return m_styles.value(states);
	
	OverlayStyle result = m_defaultStyle;
	// Check for individual states and override if defined.
	OverlayStateFlag possibleFlags[] = {
		OverlayStateFlag::Selected,
		OverlayStateFlag::Disabled,
		OverlayStateFlag::Hovered,
		OverlayStateFlag::ActiveHandle
	};
	for (OverlayStateFlag flag : possibleFlags) {
		if (states.testFlag(flag) && m_styles.contains(flag)) {
			result = m_styles.value(flag);
		}
	}
	return result;
}

// Load overlay styles from a JSON file.
// Example JSON format:
// {
//     "selected": {
//         "tintColor": "#0000FF",
//         "tintOpacity": 0.2,
//         "showOutline": true,
//         "outlineColor": "#ADD8E6",
//         "outlineThickness": 3.0,
//         "showHandles": true,
//         "handleColor": "#ADD8E6",
//         "handleSize": 10.0
//     },
//     "disabled": {
//         "tintColor": "#8B0000",
//         "tintOpacity": 0.2,
//         "showOutline": false
//     },
//     "selected+disabled": {
//         "tintColor": "#4B0082",
//         "tintOpacity": 0.8,
//         "showOutline": true,
//         "outlineColor": "#D8BFD8",
//         "outlineThickness": 2.0
//     }
// }
bool OverlayStyleSheet::loadFromFile(const QString &filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QByteArray data = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(data);
	if (!doc.isObject())
		return false;
	QJsonObject rootObj = doc.object();
	for (auto it = rootObj.begin(); it != rootObj.end(); ++it) {
		QString key = it.key();
		QJsonObject styleObj = it.value().toObject();
		QColor tint = QColor(styleObj.value("tintColor").toString());
		float opacity = static_cast<float>(styleObj.value("tintOpacity").toDouble());
		bool outline = styleObj.value("showOutline").toBool();
		QColor oColor = QColor(styleObj.value("outlineColor").toString());
		float oThickness = static_cast<float>(styleObj.value("outlineThickness").toDouble());
		bool handles = styleObj.contains("showHandles") ? styleObj.value("showHandles").toBool() : false;
		QColor hColor = styleObj.contains("handleColor") ? QColor(styleObj.value("handleColor").toString()) : QColor();
		float hSize = styleObj.contains("handleSize") ? static_cast<float>(styleObj.value("handleSize").toDouble()) : 0.0f;
		OverlayStyle style(tint, opacity, outline, oColor, oThickness, handles, hColor, hSize);
		
		// Parse the key into individual states.
		OverlayStates states;
		QStringList tokens = key.split('+', Qt::SkipEmptyParts);
		for (QString token : tokens) {
			token = token.trimmed().toLower();
			if (token == "selected")
				states |= OverlayStateFlag::Selected;
			else if (token == "disabled")
				states |= OverlayStateFlag::Disabled;
			else if (token == "hovered")
				states |= OverlayStateFlag::Hovered;
			else if (token == "activehandle")
				states |= OverlayStateFlag::ActiveHandle;
		}
		addStyle(states, style);
	}
	return true;
}
