#include "TestEval.hpp"

#include "eval/ASTBuilder.hpp"
#include "eval/ASTNode.hpp"
#include "eval/ASTNodeType.hpp"
#include "eval/CompiledExpression.hpp"
#include "eval/ExpressionContext.hpp"
#include "eval/ExpressionEvaluator.hpp"
#include "eval/Response.hpp"
#include "eval/Token.hpp"
#include "eval/Tokenizer.hpp"

#include "test/Utility.hpp"


#include <QQueue>



void TestEval::testFunctionRegistry_data() {
	QTest::addColumn<QString>("functionName");
	QTest::addColumn<QVector<qreal>>("arguments");
	QTest::addColumn<qreal>("expectedResult");
	QTest::addColumn<bool>("expectedSuccess");
	QTest::addColumn<QString>("expectedMessage");
	
	auto addFunction = [](FunctionRegistry &registry, const QString &name, std::function<qreal(const QVector<qreal> &)> func, int arity) {
		registry.registerFunction(name, func, arity);
	};
	
	FunctionRegistry registry;
	addFunction(registry, "add", [](const QVector<qreal> &args) { return args[0] + args[1]; }, 2);
	addFunction(registry, "square", [](const QVector<qreal> &args) { return args[0] * args[0]; }, 1);
	
	QTest::newRow("call valid function add")
		<< QString("add") << QVector<qreal>{3.0, 5.0} << 8.0 << true << QString();
	
	QTest::newRow("call valid function square")
		<< QString("square") << QVector<qreal>{4.0} << 16.0 << true << QString();
	
	QTest::newRow("call unknown function")
		<< QString("unknown") << QVector<qreal>{1.0, 2.0} << 0.0 << false << "Function 'unknown' not found.";
	
	QTest::newRow("call function with insufficient arguments")
		<< QString("add") << QVector<qreal>{3.0} << 0.0 << false << "Function 'add' expected 2 arguments, but got 1.";
	
	QTest::newRow("call function with excessive arguments")
		<< QString("square") << QVector<qreal>{2.0, 3.0} << 0.0 << false << "Function 'square' expected 1 arguments, but got 2.";
}

void TestEval::testFunctionRegistry() {
	QFETCH(QString, functionName);
	QFETCH(QVector<qreal>, arguments);
	QFETCH(qreal, expectedResult);
	QFETCH(bool, expectedSuccess);
	QFETCH(QString, expectedMessage);
	
	FunctionRegistry registry;
	// Register functions
	registry.registerFunction("add", [](const QVector<qreal> &args) { return args[0] + args[1]; }, 2);
	registry.registerFunction("square", [](const QVector<qreal> &args) { return args[0] * args[0]; }, 1);
	
	auto func = registry.lookupFunction(functionName);
	if (!func) {
		// Function not found
		if (expectedSuccess) {
			qDebug() << "Unexpected failure to find function:" << functionName;
		}
		QCOMPARE(false, expectedSuccess);
		QCOMPARE(QString("Function '%1' not found.").arg(functionName), expectedMessage);
		return;
	}
	
	int arity = registry.functionArity(functionName);
	if (arity != arguments.size()) {
		if (expectedSuccess) {
			qDebug() << "Unexpected arity mismatch for function:" << functionName;
		}
		QCOMPARE(false, expectedSuccess);
		QCOMPARE(QString("Function '%1' expected %2 arguments, but got %3.")
					 .arg(functionName)
					 .arg(arity)
					 .arg(arguments.size()),
				 expectedMessage);
		return;
	}
	
	// Call the function
	qreal result = func(arguments);
	if (!expectedSuccess) {
		qDebug() << "Unexpected success for function:" << functionName;
	}
	QCOMPARE(result, expectedResult);
	QCOMPARE(true, expectedSuccess);
}


QVector<Token> createTokenList(std::initializer_list<Token> tokens) {
	QVector<Token> tokenList;
	for (const auto &token : tokens) {
		tokenList.append(token);
	}
	return tokenList;
}


void TestEval::testTokenizer_data() {
	QTest::addColumn<QString>("source");
	QTest::addColumn<QVector<Token>>("expectedTokens");
	QTest::addColumn<bool>("expectedSuccess");
	QTest::addColumn<QString>("expectedMessage");
	
	// Valid cases
	QTest::newRow("basic addition")
		<< "1 + 2"
		<< createTokenList({
			Token{TokenType::Number, "1"},
			Token{TokenType::Whitespace, " "},
			Token{TokenType::Operator, "+"},
			Token{TokenType::Whitespace, " "},
			Token{TokenType::Number, "2"},
			Token{TokenType::End, ""}
		   })
		<< true << QString();
	
	QTest::newRow("simple parentheses")
		<< "(1 + 2)"
		<< createTokenList({
			Token{TokenType::Parenthesis, "("},
			Token{TokenType::Number, "1"},
			Token{TokenType::Whitespace, " "},
			Token{TokenType::Operator, "+"},
			Token{TokenType::Whitespace, " "},
			Token{TokenType::Number, "2"},
			Token{TokenType::Parenthesis, ")"},
			Token{TokenType::End, ""}
		   })
		<< true << QString();
	
	QTest::newRow("function call with single argument")
		<< "sin(0)"
		<< createTokenList({
			Token{TokenType::Name, "sin"},
			Token{TokenType::Parenthesis, "("},
			Token{TokenType::Number, "0"},
			Token{TokenType::Parenthesis, ")"},
			Token{TokenType::End, ""}
		   })
		<< true << QString();
	QTest::newRow("function call with multiple argument")
		<< "pow(1, 2)"
		<< createTokenList({
			Token{TokenType::Name, "pow"},
			Token{TokenType::Parenthesis, "("},
			Token{TokenType::Number, "1"},
			Token{TokenType::Delimiter, ","},
			Token{TokenType::Whitespace, " "},
			Token{TokenType::Number, "2"},
			Token{TokenType::Parenthesis, ")"},
			Token{TokenType::End, ""}
		   })
		<< true << QString();
	// Invalid cases
	QTest::newRow("invalid character")
		<< "1 + ?"
		<< QVector<Token>() // Expected empty token list
		<< false << "Invalid character in input: '?'";

	QTest::newRow("mismatched parentheses")
		<< "(1 + 2"
		<< QVector<Token>() // Expected empty token list
		<< false << "Mismatched parentheses.";
}



void TestEval::testTokenizer() {
	QFETCH(QString, source);
	QFETCH(QVector<Token>, expectedTokens);
	QFETCH(bool, expectedSuccess);
	QFETCH(QString, expectedMessage);
	
	QVector<Token> output;
	Tokenizer tokenizer;
	auto response = tokenizer.tokenize(source, output);
	
	if (response->success != expectedSuccess) {
		qDebug().noquote().nospace() << "         unexpected status";
		qDebug().noquote().nospace() << " Source: " << source;
		qDebug().noquote().nospace() << "Success: expected = " << expectedSuccess << ", actual = " << response->success;
		qDebug().noquote().nospace() << "Message: " << response->message;
	}
	QCOMPARE(response->success, expectedSuccess);
	
	if (expectedSuccess) {
		if (output != expectedTokens) {
			qDebug().noquote().nospace() << "         unexpected result";
			qDebug().noquote().nospace() << " Source: " << source;
			qDebug().noquote().nospace() << "    Value: expected = " << expectedTokens << ", actual = " << output;
		}
		QCOMPARE(output, expectedTokens);
	} else {
		if (response->message != expectedMessage) {
			qDebug().noquote().nospace() << "         unexpected error message";
			qDebug().noquote().nospace() << " Source: " << source;
			qDebug().noquote().nospace() << "Message: expected = " << expectedMessage << ", actual = " << response->message;
		}
		QCOMPARE(response->message, expectedMessage);
	}
}




inline bool operator==(const QQueue<Token> &lhs, const QQueue<Token> &rhs) {
	const auto sz = lhs.size();
	if (sz != rhs.size()) {
		return false;
	}
	for (qsizetype i = 0; i < sz; ++i) {
		auto lht = lhs[i];
		auto rht = rhs[i];
		
		if (lht != rht) {
			return false;
		}
	}
	return true;
}


QQueue<Token> createQueue(std::initializer_list<Token> tokens) {
	QQueue<Token> queue;
	for (const auto &token : tokens) {
		queue.enqueue(token);
	}
	return queue;
}


void TestEval::testRPN_data() {
	QTest::addColumn<QVector<Token>>("tokens");
	QTest::addColumn<QQueue<Token>>("expectedRPN");
	QTest::addColumn<bool>("expectedSuccess");
	QTest::addColumn<QString>("expectedMessage");
	
	// Valid cases
	QTest::newRow("basic addition")
		<< QVector<Token>{
						  Token{TokenType::Number, "1"},
						  Token{TokenType::Operator, "+"},
						  Token{TokenType::Number, "2"}}
		<< createQueue({
						Token{TokenType::Number, "1"},
						Token{TokenType::Number, "2"},
						Token{TokenType::Operator, "+"}})
		<< true << QString();
	
	QTest::newRow("precedence and associativity")
		<< QVector<Token>{
						  Token{TokenType::Number, "1"},
						  Token{TokenType::Operator, "+"},
						  Token{TokenType::Number, "2"},
						  Token{TokenType::Operator, "*"},
						  Token{TokenType::Number, "3"}}
		<< createQueue({
						Token{TokenType::Number, "1"},
						Token{TokenType::Number, "2"},
						Token{TokenType::Number, "3"},
						Token{TokenType::Operator, "*"},
						Token{TokenType::Operator, "+"}})
		<< true << QString();
	
	QTest::newRow("parentheses")
		<< QVector<Token>{
						  Token{TokenType::Parenthesis, "("},
						  Token{TokenType::Number, "1"},
						  Token{TokenType::Operator, "+"},
						  Token{TokenType::Number, "2"},
						  Token{TokenType::Parenthesis, ")"},
						  Token{TokenType::Operator, "*"},
						  Token{TokenType::Number, "3"}}
		<< createQueue({
						Token{TokenType::Number, "1"},
						Token{TokenType::Number, "2"},
						Token{TokenType::Operator, "+"},
						Token{TokenType::Number, "3"},
						Token{TokenType::Operator, "*"}})
		<< true << QString();
	
	// Invalid cases
	QTest::newRow("mismatched parentheses")
		<< QVector<Token>{
						  Token{TokenType::Parenthesis, "("},
						  Token{TokenType::Number, "1"},
						  Token{TokenType::Operator, "+"},
						  Token{TokenType::Number, "2"}}
		<< QQueue<Token>() // Expected empty queue
		<< false << "Mismatched parentheses.";
	
	QTest::newRow("invalid token")
		<< QVector<Token>{
						  Token{TokenType::Number, "1"},
						  Token{TokenType::Operator, "+"},
						  Token{TokenType::Invalid, "?"}}
		<< QQueue<Token>() // Expected empty queue
		<< false << "Invalid token encountered: '?'";
}



void TestEval::testRPN() {
	QFETCH(QVector<Token>, tokens);
	QFETCH(QQueue<Token>, expectedRPN);
	QFETCH(bool, expectedSuccess);
	QFETCH(QString, expectedMessage);
	
	QQueue<Token> output;
	auto response = rpn(tokens, output);
	if (response->success != expectedSuccess) {
		qDebug().noquote().nospace() << "         unexpected status";
		qDebug().noquote().nospace() << " Tokens: " << tokens;
		qDebug().noquote().nospace() << "Success: expected = " << expectedSuccess << ", actual = " << response->success;
		qDebug().noquote().nospace() << "Message: " << response->message;
	}
	QCOMPARE(response->success, expectedSuccess);
	if (expectedSuccess) {
		if (output != expectedRPN) {
			qDebug().noquote().nospace() << "         unexpected result";
			qDebug().noquote().nospace() << " Tokens: " << tokens;
			qDebug().noquote().nospace() << "    Value: expected = " << expectedRPN << ", actual = " << output;
		}
		QCOMPARE(output, expectedRPN);
	} else {
		if (response->message != expectedMessage) {
			qDebug().noquote().nospace() << "         unexpected error message";
			qDebug().noquote().nospace() << " Tokens: " << tokens;
			qDebug().noquote().nospace() << "Message: expected = " << expectedMessage << ", actual = " << response->message;
		}
		QCOMPARE(response->message, expectedMessage);
	}
}





void TestEval::testAST_data() {
	QTest::addColumn<QQueue<Token>>("rpnQueue");
	QTest::addColumn<QSharedPointer<ASTNode>>("expectedAST");
	QTest::addColumn<bool>("expectedSuccess");
	QTest::addColumn<QString>("expectedMessage");
	
	auto createNode = [](ASTNodeType type, const QString &value, QVector<QSharedPointer<ASTNode>> children = {}) {
		auto node = QSharedPointer<ASTNode>::create();
		node->type = type;
		node->value = value;
		node->children = children;
		return node;
	};
	
	// Valid cases
	QTest::newRow("basic addition")
		<< createQueue({
						Token{TokenType::Number, "1"},
						Token{TokenType::Number, "2"},
						Token{TokenType::Operator, "+"}})
		<< createNode(
			   ASTNodeType::Operator, "+",
			   {createNode(ASTNodeType::Number, "1"), createNode(ASTNodeType::Number, "2")})
		<< true << QString();
	
	QTest::newRow("precedence")
		<< createQueue({
						Token{TokenType::Number, "1"},
						Token{TokenType::Number, "2"},
						Token{TokenType::Number, "3"},
						Token{TokenType::Operator, "*"},
						Token{TokenType::Operator, "+"}})
		<< createNode(
			   ASTNodeType::Operator, "+",
			   {createNode(ASTNodeType::Number, "1"),
				createNode(ASTNodeType::Operator, "*",
						   {createNode(ASTNodeType::Number, "2"), createNode(ASTNodeType::Number, "3")})})
		<< true << QString();
}


void TestEval::testAST() {
	QFETCH(QQueue<Token>, rpnQueue);
	QFETCH(QSharedPointer<ASTNode>, expectedAST);
	QFETCH(bool, expectedSuccess);
	QFETCH(QString, expectedMessage);
	
	QSharedPointer<ASTNode> astRoot;
	auto response = ast(rpnQueue, astRoot);
	if (response->success != expectedSuccess) {
		qDebug().noquote().nospace() << "           unexpected status";
		qDebug().noquote().nospace() << "RPN Queue: " << rpnQueue;
		qDebug().noquote().nospace() << "  Success: expected = " << expectedSuccess << ", actual = " << response->success;
		qDebug().noquote().nospace() << "  Message: " << response->message;
	}
	QCOMPARE(response->success, expectedSuccess);
	if (expectedSuccess) {
		const auto equal = *astRoot.data() == *expectedAST.data();
		if (!equal) {
			qDebug().noquote().nospace() << "           unexpected result";
			qDebug().noquote().nospace() << "RPN Queue: " << rpnQueue;
			qDebug().noquote().nospace() << "    Value: expected = " << expectedAST << ", actual = " << astRoot;
		}
		QVERIFY(equal);
	} else {
		const auto equal = response->message == expectedMessage;
		if (!equal) {
			qDebug().noquote().nospace() << "           unexpected error message";
			qDebug().noquote().nospace() << "RPN Queue: " << rpnQueue;
			qDebug().noquote().nospace() << "  Message: expected = " << expectedMessage << ", actual = " << response->message;
		}
		QVERIFY(equal);
	}
}




typedef QMap<QString, qreal> VariablesMap;

	
	
void TestEval::prepareData(){
	QTest::addColumn<QString>("expression");
	QTest::addColumn<VariablesMap>("variables");
	QTest::addColumn<bool>("expectedSuccess");
	QTest::addColumn<qreal>("expectedValue");
	QTest::addColumn<QString>("expectedMessage");
	
	
	QStringList spaceAlternatives;
	spaceAlternatives <<""<<" "<<"	"<<"	"<<"	 "<<"\n"<<"\n "<<" \n"<<"\n	"<<"	\n";
	int num=0;
	for(const auto &space1:spaceAlternatives){
		for(const auto &space2:spaceAlternatives){
			for(const auto &space3:spaceAlternatives){
				for(const auto &space4:spaceAlternatives){
					num ++ ;
					const auto name = QString("space test %1").arg(num);
					const auto expression = space1 + "1"+ space2 + "+" + space3 + "2" + space4;
					//QTest::newRow(name.toStdString().c_str()) << expression << QMap<QString, qreal>{} << true << 3.0 << QString();
				}
			}
		}
	}
	// Basic arithmetic
	QTest::newRow("basic addition") << "1 + 2" << QMap<QString, qreal>{} << true << 3.0 << QString();
	QTest::newRow("subtraction") << "10 - 3" << QMap<QString, qreal>{} << true << 7.0 << QString();
	QTest::newRow("multiplication") << "3 * 4" << QMap<QString, qreal>{} << true << 12.0 << QString();
	QTest::newRow("division") << "10 / 2" << QMap<QString, qreal>{} << true << 5.0 << QString();
	
	// Parentheses and precedence
	QTest::newRow("simple precedence") << "2 + 3 * 4" << QMap<QString, qreal>{} << true << 14.0 << QString();
	QTest::newRow("parentheses and precedence") << "(2 + 3) * 4" << QMap<QString, qreal>{} << true << 20.0 << QString();
	QTest::newRow("nested parentheses") << "10 / (5 - 3)" << QMap<QString, qreal>{} << true << 5.0 << QString();
	
	// Functions
	QTest::newRow("sin function") << "sin(0)" << QMap<QString, qreal>{} << true << 0.0 << QString();
	QTest::newRow("cos function") << "cos(0)" << QMap<QString, qreal>{} << true << 1.0 << QString();
	QTest::newRow("tan function") << "tan(0)" << QMap<QString, qreal>{} << true << 0.0 << QString();
	QTest::newRow("log function") << "log(1)" << QMap<QString, qreal>{} << true << 0.0 << QString();
	QTest::newRow("pow function") << "pow(3, 3)" << QMap<QString, qreal>{} << true << 27.0 << QString();
	QTest::newRow("many functions") << "pow(sin(X), pow(Y, 3)) * 5" << QMap<QString, qreal>{} << true << 27.0 << QString();
	
	// Variables
	QTest::newRow("simple variable addition") << "X + Y" << QMap<QString, qreal>{{"X", 1.0}, {"Y", 2.0}} << true << 3.0 << QString();
	QTest::newRow("variable with parentheses") << "X * (Y + Z)" << QMap<QString, qreal>{{"X", 2.0}, {"Y", 3.0}, {"Z", 4.0}} << true << 14.0 << QString();
	
	// Errors
	QTest::newRow("division by zero") << "10 / 0" << QMap<QString, qreal>{} << false << 0.0 << "Division by zero";
	QTest::newRow("undefined variable") << "X + Y" << QMap<QString, qreal>{{"X", 1.0}} << false << 0.0 << "Undefined variable: Y";
	QTest::newRow("syntax error") << "1 + * 2" << QMap<QString, qreal>{} << false << 0.0 << "Unexpected token: *";
	QTest::newRow("mismatched parentheses") << "(1 + 2" << QMap<QString, qreal>{} << false << 0.0 << "Mismatched parentheses";
	QTest::newRow("unsupported function") << "inexistant(2, 3)" << QMap<QString, qreal>{} << false << 0.0 << "Unsupported function: inexistant";
	
	// Registered functions
	QTest::newRow("custom registered function") << "test(X, 2) + sin(Y)" << QMap<QString, qreal>{{"X", 3.0}, {"Y", 0.0}} << true << 5.0 << QString();
	
	// Complex expressions
	QTest::newRow("complex variable expression") << "pow(X, 2) + sin(Y)" << QMap<QString, qreal>{{"X", 3.0}, {"Y", 0.0}} << true << 9.0 << QString();
}



void TestEval::testEval_data() {
	prepareData();
}


void TestEval::testEval() {
	QFETCH(QString, expression);
	QFETCH(VariablesMap, variables);
	QFETCH(bool, expectedSuccess);
	QFETCH(qreal, expectedValue);
	QFETCH(QString, expectedMessage);
	
	ExpressionContext context;
	
	// Set variables
	for (auto it = variables.begin(); it != variables.end(); ++it) {
		context.setVariable(it.key(), it.value());
	}
	ExpressionEvaluator evaluator;
	qreal result = 0.0;
	auto response = evaluator.eval(context, expression, result);
	if (response->success != expectedSuccess) {
		qDebug().noquote().nospace() << "            unexpected status";
		qDebug().noquote().nospace() << "Expression: " << expression;
		qDebug().noquote().nospace() << "   Success: expected = " << expectedSuccess << ", actual = " << response->success;
		qDebug().noquote().nospace() << "   Message: " << response->message;
	}
	
	QCOMPARE(response->success, expectedSuccess);
	if (response->success) {
		if (!qFuzzyCompare(result, expectedValue)) {
			qDebug().noquote().nospace() << "            unexpected result";
			qDebug().noquote().nospace() << "Expression: " << expression;
			qDebug().noquote().nospace() << "     Value: expected = " << expectedValue << ", actual = " << result;
		}
		QVERIFY(qFuzzyCompare(result, expectedValue));
	} else {
		if (response->message != expectedMessage) {
			qDebug().noquote().nospace() << "            unexpected error message";
			qDebug().noquote().nospace() << "Expression: " << expression;
			qDebug().noquote().nospace() << "   Message: expected = " << expectedMessage << ", actual = " << response->message;
		}
		QCOMPARE(response->message, expectedMessage);
	}
}

void TestEval::testCompile_data() {
	prepareData();
}


void TestEval::testCompile() {
	QFETCH(QString, expression);
	QFETCH(VariablesMap, variables);
	QFETCH(bool, expectedSuccess);
	QFETCH(qreal, expectedValue);
	QFETCH(QString, expectedMessage);
	
	ExpressionContext context;
	
	// Set variables
	for (auto it = variables.begin(); it != variables.end(); ++it) {
		context.setVariable(it.key(), it.value());
	}
	ExpressionEvaluator evaluator;
	QSharedPointer<CompiledExpression> compiledExpression;
	auto response = evaluator.compile(expression, compiledExpression);
	if (response->success != expectedSuccess) {
		qDebug().noquote().nospace() << "            unexpected status";
		qDebug().noquote().nospace() << "Expression: " << expression;
		qDebug().noquote().nospace() << "   Success: expected = " << expectedSuccess << ", actual = " << response->success;
		qDebug().noquote().nospace() << "   Message: " << response->message;
		qDebug().noquote().nospace() << "  Compiled: " << compiledExpression;
	}
	QCOMPARE(response->success, expectedSuccess);
	if (response->success) {
		QVERIFY(!compiledExpression.isNull());
		
		// Execute the compiled expression
		// Execute the compiled expression
		qreal result{0.0};
		auto executeResponse = compiledExpression->execute(context, result);
		if(!executeResponse.isNull() && !executeResponse->success){
			// TODO: Handle this error
			qDebug() << "Execute error : " << executeResponse->message;
		}

		if (!qFuzzyCompare(result, expectedValue)) {
			qDebug().noquote().nospace() << "            unexpected result";
			qDebug().noquote().nospace() << "Expression: " << expression;
			qDebug().noquote().nospace() << "     Value: expected = " << expectedValue << ", actual = " << result;
			qDebug().noquote().nospace() << "  Compiled: " << compiledExpression;
		}
		QVERIFY(qFuzzyCompare(result, expectedValue));
	} else {
		if (response->message != expectedMessage) {
			qDebug().noquote().nospace() << "            unexpected error message";
			qDebug().noquote().nospace() << "Expression: " << expression;
			qDebug().noquote().nospace() << "   Message: expected = " << expectedMessage << ", actual = " << response->message;
			qDebug().noquote().nospace() << "  Compiled: " << compiledExpression;
		}
		QCOMPARE(response->message, expectedMessage);
	}
}



OC_TEST_MAIN(test, TestEval)
