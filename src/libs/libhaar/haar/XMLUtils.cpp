#include "XMLUtils.hpp"

#include <QRegularExpression>




QString printXmlStructure(const QDomElement &element, int indentLevel) {
	QString output;
	QString indent = QString(indentLevel * 3, ' ');
	
	// Print current element
	output += indent + element.tagName();
	const auto attributes = element.attributes();
	const auto asz = attributes.length();
	if(asz > 0){
		output += "(";
		for(int i=0; i<asz; ++i){
			const auto attr = attributes.item(i);
			output += attr.nodeName() + "=" + attr.nodeValue()+", ";
		}
		output += ")";
	}
	const auto tagContent{element.nodeValue().trimmed()};
	if (!tagContent.isEmpty()) {
		output += ": " + tagContent;
	}
	output += "\n";
	
	// Iterate over child elements
	QDomElement child = element.firstChildElement();
	while (!child.isNull()) {
		output += printXmlStructure(child, indentLevel + 1); // Recursive call for child
		child = child.nextSiblingElement();
	}
	
	return output;
}



void debugElement(const QDomElement &element) {
	if (element.isNull()) {
		qDebug() << "Element is null.";
		return;
	}
	
	// Element type
	QString nodeType;
	switch (element.nodeType()) {
	case QDomNode::ElementNode:
		nodeType = "ElementNode";
		break;
	case QDomNode::TextNode:
		nodeType = "TextNode";
		break;
	case QDomNode::AttributeNode:
		nodeType = "AttributeNode";
		break;
	default:
		nodeType = "UnknownNode";
		break;
	}
	
	qDebug() << "Element Details:";
	qDebug() << "  Type:" << nodeType;
	qDebug() << "  Tag Name:" << element.tagName();
	
	// Content text
	QString text = element.text().trimmed();
	qDebug() << "  Text Content:" << (text.isEmpty() ? "<empty>" : text);
	
	// Attributes
	qDebug() << "  Attributes:";
	if (element.hasAttributes()) {
		QDomNamedNodeMap attributes = element.attributes();
		for (int i = 0; i < attributes.count(); ++i) {
			QDomNode attr = attributes.item(i);
			qDebug() << "    -" << attr.nodeName() << "=" << attr.nodeValue();
		}
	} else {
		qDebug() << "    <none>";
	}
	
	// Child elements
	qDebug() << "  Child Elements:";
	QDomElement child = element.firstChildElement();
	if (!child.isNull()) {
		while (!child.isNull()) {
			qDebug() << "    -" << child.tagName();
			child = child.nextSiblingElement();
		}
	} else {
		qDebug() << "    <none>";
	}
}

QList<QDomElement> directChildrenByTagName(const QDomElement &parent, const QString &tagName) {
	QList<QDomElement> matchingChildren;
	QDomElement child = parent.firstChildElement();
	while (!child.isNull()) {
		if (child.tagName() == tagName) {
			matchingChildren.append(child);
		}
		child = child.nextSiblingElement();
	}
	return matchingChildren;
}

