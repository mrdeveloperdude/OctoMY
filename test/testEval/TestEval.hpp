#ifndef TESTEVAL_HPP
#define TESTEVAL_HPP

#include "test/Common.hpp"

class TestEval:public QObject
{
	Q_OBJECT
private:
	void prepareData();
private slots:
	void testFunctionRegistry_data();
	void testFunctionRegistry();
	void testEval_data();
	void testEval();
	void testTokenizer_data();
	void testTokenizer();
	void testRPN_data();
	void testRPN();
	void testAST_data();
	void testAST();
	void testCompile_data();
	void testCompile();
	
};


#endif
// TESTEVAL_HPP
