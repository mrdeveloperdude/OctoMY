#ifndef XMLUTILS_HPP
#define XMLUTILS_HPP

#include <QString>
#include <QDomElement>
#include <QList>

QString printXmlStructure(const QDomElement &element, int indentLevel = 0);
void debugElement(const QDomElement &element);
QList<QDomElement> directChildrenByTagName(const QDomElement &parent, const QString &tagName);


#endif // XMLUTILS_HPP
