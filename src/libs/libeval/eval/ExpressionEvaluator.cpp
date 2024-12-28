#include "ExpressionEvaluator.hpp"

#include "ASTBuilder.hpp"
#include "ASTNode.hpp"
#include "CompiledExpression.hpp"
#include "Response.hpp"
#include "Tokenizer.hpp"

QSharedPointer<Response> ExpressionEvaluator::eval(ExpressionContext &context, const QString &expression, qreal &result) {
	QSharedPointer<CompiledExpression> compiledExpression;
	auto compileResponse = compile(expression, compiledExpression);
	
	if (compileResponse && !compileResponse->success) {
		return compileResponse;
	}
	
	// Execute the compiled expression
	auto executeResponse = compiledExpression->execute(context, result);
	if(!executeResponse.isNull() && !executeResponse->success){
		return executeResponse;
	}
	
	return QSharedPointer<Response>::create();
}


QSharedPointer<Response> ExpressionEvaluator::compile(const QString &expression, QSharedPointer<CompiledExpression> &compiledExpression) {
	// Step 1: Tokenize the input expression
	QVector<Token> tokens;
	Tokenizer tokenizer;
	auto tokenizerResponse = tokenizer.tokenize(expression, tokens);
	
	if (tokenizerResponse && !tokenizerResponse->success) {
		return tokenizerResponse;
	}
	
	// Step 2: Build the AST
	QSharedPointer<ASTNode> astRoot;
	ASTBuilder astBuilder;
	auto astBuilderResponse = astBuilder.build(tokens, astRoot);
	
	if (astBuilderResponse && !astBuilderResponse->success) {
		return astBuilderResponse;
	}
	
	// Step 3: Compile the expression
	compiledExpression = QSharedPointer<CompiledExpression>::create();
	QSharedPointer<Response> compiledResponse = compiledExpression->compile(astRoot);
	
	if (compiledResponse && !compiledResponse->success) {
		return compiledResponse;
	}
	
	return QSharedPointer<Response>::create();
}
