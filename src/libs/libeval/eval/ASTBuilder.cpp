#include "ASTBuilder.hpp"

#include <QDebug>
#include <QQueue>
#include <QStack>

#include "Token.hpp"
#include "ASTNode.hpp"
#include "Response.hpp"


static inline int operatorPrecedence(const QString &op) {
	if (op == "+" || op == "-") return 1;
	if (op == "*" || op == "/") return 2;
	if (op == "%") return 2;
	return -1;
}


QSharedPointer<Response> rpn(const QVector<Token> &tokens, QQueue<Token> &output) {
	QStack<Token> operatorStack;
	QStack<int> functionArgCount; // Tracks argument counts for functions
	output.clear();
	QString whitespace;
	
	for (const auto &token : tokens) {
		if (token.type == TokenType::Number || token.type == TokenType::Name) {
			// Add numbers and variable/function names directly to the output queue
			Token outputToken = token;
			outputToken.whitespace = whitespace;
			whitespace.clear();
			output.enqueue(outputToken);
		} else if (token.type == TokenType::Operator) {
			if (operatorPrecedence(token.value) == -1) {
				return QSharedPointer<Response>::create(QString("Invalid operator encountered: '%1'").arg(token.value));
			}
			// Handle operators based on precedence and associativity
			while (!operatorStack.isEmpty()) {
				const Token &top = operatorStack.top();
				if (top.type == TokenType::Operator &&
					(operatorPrecedence(top.value) > operatorPrecedence(token.value) ||
					 (operatorPrecedence(top.value) == operatorPrecedence(token.value)))) {
					output.enqueue(operatorStack.pop());
				} else {
					break;
				}
			}
			operatorStack.push(token);
		} else if (token.type == TokenType::Parenthesis && token.value == "(") {
			operatorStack.push(token);
			// Initialize argument count if the previous token is a function name
			if (!output.isEmpty() && output.back().type == TokenType::Name) {
				functionArgCount.push(0); // Start counting arguments for the function
			}
		} else if (token.type == TokenType::Parenthesis && token.value == ")") {
			// Pop operators until an opening parenthesis is found
			while (!operatorStack.isEmpty() && operatorStack.top().value != "(") {
				output.enqueue(operatorStack.pop());
			}
			if (operatorStack.isEmpty() || operatorStack.pop().value != "(") {
				return QSharedPointer<Response>::create("Mismatched parentheses.");
			}
			// Handle function tokens
			if (!functionArgCount.isEmpty()) {
				if (functionArgCount.top() == 0) {
					functionArgCount.top()++; // Handle single argument case
				}
				Token functionToken = output.dequeue();
				functionToken.arity = functionArgCount.pop();
				output.enqueue(functionToken);
			}
		} else if (token.type == TokenType::Delimiter) {
			// Argument delimiters increment argument count for the current function
			if (functionArgCount.isEmpty()) {
				return QSharedPointer<Response>::create("Unexpected argument delimiter outside of function.");
			}
			functionArgCount.top()++;
		} else if (token.type == TokenType::Whitespace) {
			whitespace += token.value;
		} else if (token.type == TokenType::End) {
			break;
		} else {
			return QSharedPointer<Response>::create(QString("Invalid token encountered: '%1'").arg(token.value));
		}
	}
	
	// Pop all remaining operators onto the output queue
	while (!operatorStack.isEmpty()) {
		Token top = operatorStack.pop();
		if (top.type == TokenType::Parenthesis) {
			return QSharedPointer<Response>::create("Mismatched parentheses.");
		}
		output.enqueue(top);
	}
	
	return QSharedPointer<Response>::create();
}

static void logTokenQueue(const QQueue<Token> &queue){
	qDebug()<<"token queue: "<<queue.size();
	for (const auto &token : queue) {
		qDebug()<<" + "<<token;
	}
}


static void logNodeStack(const QStack<QSharedPointer<ASTNode>> &stack){
	qDebug()<<"ast node stack: "<<stack.size();
	for (const auto &node : stack) {
		qDebug()<<" + "<<node;
	}
}



static int functionArity(const QString &functionName){
	// Add actual function arity resolution logic here
	if (functionName == "sin") return 1;
	if (functionName == "pow") return 2;
	return -1; // Unknown function
};


QSharedPointer<Response> ast(const QQueue<Token> &rpnQueue, QSharedPointer<ASTNode> &astRoot) {
	QStack<QSharedPointer<ASTNode>> stack;
	
	for (const auto &token : rpnQueue) {
		if (token.type == TokenType::Number) {
			auto node = QSharedPointer<ASTNode>::create();
			node->type = ASTNodeType::Number;
			node->value = token.value;
			stack.push(node);
		} else if (token.type == TokenType::Name && token.arity > 0) {
			if (stack.size() < token.arity) {
				return QSharedPointer<Response>::create(QString("Insufficient operands for function '%1'.").arg(token.value));
			}
			auto node = QSharedPointer<ASTNode>::create();
			node->type = ASTNodeType::Function;
			node->value = token.value;
			
			// Pop arguments from the stack and add them as children
			for (int i = 0; i < token.arity; ++i) {
				node->children.prepend(stack.pop());
			}
			stack.push(node);
		} else if (token.type == TokenType::Operator) {
			if (stack.size() < 2) {
				return QSharedPointer<Response>::create(QString("Insufficient operands for operator '%1'.").arg(token.value));
			}
			
			auto right = stack.pop();
			auto left = stack.pop();
			
			auto node = QSharedPointer<ASTNode>::create();
			node->type = ASTNodeType::Operator;
			node->value = token.value;
			node->children.append(left);
			node->children.append(right);
			
			stack.push(node);
		} else {
			return QSharedPointer<Response>::create(QString("Invalid token '%1' in RPN expression.").arg(token.value));
		}
	}
	
	if (stack.size() != 1) {
		logTokenQueue(rpnQueue);
		logNodeStack(stack);
		return QSharedPointer<Response>::create("Invalid RPN expression: stack size is not 1 after processing.");
	}
	
	astRoot = stack.pop();
	return QSharedPointer<Response>::create();
}




QSharedPointer<Response> ASTBuilder::build(const QVector<Token> &tokens, QSharedPointer<ASTNode> &astRoot) {
	astRoot.clear();
	if (tokens.isEmpty()) {
		return QSharedPointer<Response>::create("Token stream is empty.");
	}
	QQueue<Token> rpnQueue;
	auto response = rpn(tokens, rpnQueue);
	if (!response->success) {
		return response;
	}
	return ast(rpnQueue, astRoot);
}
