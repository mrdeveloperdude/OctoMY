#ifndef QVARIANTPRETTYPRINTER_HPP
#define QVARIANTPRETTYPRINTER_HPP


#include <QVariant>
#include <QString>
#include <QVariantMap>
#include <QVariantList>

class QVariantPrettyPrinter
{
private:
	QString m_indentStr;
public:
	QVariantPrettyPrinter(const QString &indentStr = "\t");
	QString prettyPrint(const QVariantMap &map) const;
	
private:
	QString prettyPrintInternal(const QVariant &value, const QString &currentIndent) const;
};



#endif // QVARIANTPRETTYPRINTER_HPP
