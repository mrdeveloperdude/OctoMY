#ifndef ASTNODE_HPP
#define ASTNODE_HPP

#include "ASTNodeType.hpp"

#include <QString>
#include <QVector>
#include "uptime/SharedPointerWrapper.hpp"

struct ASTNode {
	ASTNodeType type;
	QString value;
	QVector<QSharedPointer<ASTNode>> children;
};


inline bool operator==(const ASTNode &lhs, const ASTNode &rhs) {
	if (lhs.type != rhs.type || lhs.value != rhs.value) {
		return false;
	}
	
	if (lhs.children.size() != rhs.children.size()) {
		return false;
	}
	
	for (qsizetype i = 0; i < lhs.children.size(); ++i) {
		if (!lhs.children[i] || !rhs.children[i]) {
			if (lhs.children[i] != rhs.children[i]) {
				return false;
			}
		} else {
			if (!(*(lhs.children[i]) == *(rhs.children[i]))) {
				return false;
			}
		}
	}
	
	return true;
}


inline bool operator!=(const ASTNode &lhs, const ASTNode &rhs) {
	return !(lhs == rhs);
}


void printASTNode(QDebug &debug, const QSharedPointer<ASTNode> &node, int indentLevel);

QDebug operator<<(QDebug debug, const QSharedPointer<ASTNode> &node);

#endif // ASTNODE_HPP
