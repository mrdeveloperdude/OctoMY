#include "ASTNode.hpp"

#include <QDebug>


void printASTNode(QDebug &debug, const QSharedPointer<ASTNode> &node, int indentLevel) {
	if (!node) {
		debug << "NIL\n";
		return;
	}
	QString indent(indentLevel * 2, ' '); // 2 spaces per indent level
	debug << indent << "[" << node->type << ": " << node->value << "]\n";
	for (const auto &child : node->children) {
		printASTNode(debug, child, indentLevel + 1);
	}
}


QDebug operator<<(QDebug debug, const QSharedPointer<ASTNode> &node) {
	debug.nospace() << "ASTNode Tree:\n";
	printASTNode(debug.noquote().nospace(), node, 0);
	return debug.space();
}
