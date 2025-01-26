#include "CompiledExpression.hpp"

#include "ASTNode.hpp"
#include "Response.hpp"
#include "ExpressionContext.hpp"

#include <QDebug>
#include <cmath>
#include <functional>

// TODO: This entire expression lib is chatgpt slop that needs to be scrutinized and gone over thoroughly

QSharedPointer<Response> CompiledExpression::compile(const QString &original, const QSharedPointer<ASTNode> &astRoot) {
	if (!astRoot) {
		return QSharedPointer<Response>::create("AST root is null.");
	}
	this->original = original;
	// Store the root node for execution
	rootNode = astRoot;
	return QSharedPointer<Response>::create();
}

QSharedPointer<Response> CompiledExpression::execute(ExpressionContext &context, qreal &value) const {
	if (rootNode.isNull()) {
		return QSharedPointer<Response>::create("Expression tree is empty.");
	}
	// Lambda to evaluate nodes recursively
	std::function<qreal(const QSharedPointer<ASTNode> &, QSharedPointer<Response> &)> evaluateNode;
	evaluateNode = [&](const QSharedPointer<ASTNode> &node, QSharedPointer<Response> &response) -> qreal {
		if (node.isNull()) {
			response = QSharedPointer<Response>::create("Invalid null node encountered.");
			return 0.0;
		}
		
		switch (node->type) {
		case ASTNodeType::Number:
			return node->value.toDouble();
		case ASTNodeType::Operator: {
			if (node->children.size() != 2) {
				response = QSharedPointer<Response>::create("Operator node has invalid children count.");
				return 0.0;
			}
			qreal left = evaluateNode(node->children[0], response);
			if (!response->success) return 0.0;
			qreal right = evaluateNode(node->children[1], response);
			if (!response->success) return 0.0;
			
			if (node->value == "+") return left + right;
			if (node->value == "-") return left - right;
			if (node->value == "*") return left * right;
			if (node->value == "/") return (right != 0.0) ? left / right : (response = QSharedPointer<Response>::create("Division by zero."), 0.0);
			if (node->value == "%") return (right != 0.0) ? std::fmod(left, right) : (response = QSharedPointer<Response>::create("Modulo by zero."), 0.0);
			
			response = QSharedPointer<Response>::create(QString("Unsupported operator: '%1'.").arg(node->value));
			return 0.0;
		}
		case ASTNodeType::Variable: {
			if (!context.hasVariable(node->value)) {
				response = QSharedPointer<Response>::create(QString("Variable '%1' not found.").arg(node->value));
				return 0.0;
			}
			return context.variable(node->value);
		}
		case ASTNodeType::Function: {
			auto func = context.functionRegistry().lookupFunction(node->value);
			if (!func) {
				response = QSharedPointer<Response>::create(QString("Function '%1' not found.").arg(node->value));
				return 0.0;
			}
			
			QVector<qreal> args;
			for (const auto &child : node->children) {
				args.append(evaluateNode(child, response));
				if (!response->success) return 0.0;
			}
			return func(args);
		}
		default:
			response = QSharedPointer<Response>::create(QString("Unsupported node type: '%1'.").arg(ASTNodeTypeToString(node->type)));
			return 0.0;
		}
	};
	// Initialize response and value
	QSharedPointer<Response> response = QSharedPointer<Response>::create();
	value = evaluateNode(rootNode, response);
	return response;
}



QDebug operator<<(QDebug debug, const QSharedPointer<CompiledExpression> &cexp) {
	debug.nospace() << "CompiledExpression:\n";
	if(!cexp){
		debug.nospace() << "NIL\n";
	}
	else{
		debug << cexp->rootNode;
	}
	return debug.space();
}

