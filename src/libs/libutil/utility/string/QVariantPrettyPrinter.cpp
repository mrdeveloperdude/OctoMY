#include "QVariantPrettyPrinter.hpp"


QVariantPrettyPrinter::QVariantPrettyPrinter(const QString &indentStr)
	: m_indentStr(indentStr)
{
}


QString QVariantPrettyPrinter::prettyPrint(const QVariantMap &map) const
{
	return prettyPrintInternal(map, 0);
}


QString QVariantPrettyPrinter::prettyPrintInternal(const QVariant &value, const QString &currentIndent) const
{
	QString result;
	QString indent = currentIndent + m_indentStr;  // Indent one level deeper
	if (value.metaType().id() == QMetaType::QVariantMap) {
		const QVariantMap map = value.toMap();
		if(map.size() <= 0){
			result += "{}\n";
		}
		else{
			result += currentIndent + "{\n";
			for (auto it = map.cbegin(); it != map.cend(); ++it) {
				result += indent + "\"" + it.key() + "\" : " + prettyPrintInternal(it.value(), indent);
			}
			result += currentIndent + "}\n";
		}
	} else if (value.metaType().id() == QMetaType::QVariantList) {
		const QVariantList list = value.toList();
		if(list.size() <= 0){
			result += "[]\n";
		}
		else{
			result += currentIndent + "[\n";
			for (const auto &item : list) {
				result += prettyPrintInternal(item, indent);
			}
			result += currentIndent + "]\n";
		}
	} else {
		if (value.metaType().id() == QMetaType::QString) {
			result += "\"" + value.toString() + "\"\n";
		} else if (value.metaType().id() == QMetaType::Bool) {
			result += (value.toBool() ? "true" : "false") + QString("\n");
		} else {
			result += value.toString() + "\n";
		}
	}
	return result;
}

