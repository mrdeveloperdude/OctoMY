-- NOTE: Why don't we simplify the build by using merged_output option to form
--       one file instead of 4? Because this file becomes impossible to use,
--       since we cannot include it without causing duplicate symbols being
--       linked and we cannot link it without manually specifying a header for
--       it.

-- This gives a name to the table. Default is simply "parser"
-- Generates calc_table.cpp & calc_table_p.h
%parser calc_table
-- This gives filenames to the parser itself
%decl calc_parser.hpp
%impl calc_parser.cpp

%token_prefix TOK_
%token number
%token DESCRIPTOR
%token PLAN
%token LPAREN
%token RPAREN
%token LBRACKET
%token RBRACKET
%token LBRACE
%token RBRACE
%token STRING

%start Goal

/:

#ifndef CALC_PARSER_HPP
#define CALC_PARSER_HPP

#include "calc_table_p.h"
#include "plan/parser/GeneralPurposeParser.hpp"


struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;

class CalcLexer{
	private:
		YY_BUFFER_STATE buf;
	public:

		bool init(const char *in);
		bool deinit();

};

class CalcParser: public GeneralPurposeParser<CalcLexer, $table>{
	private:
		int stack[10];
	public:

		CalcParser(){
			for(int i=0;i<10;++i){
				stack[i]=2;

			}

		}

		int nextToken();
		void consumeRule(int rule);

		int &sym(int index){
			if(index<1){
				index=1;
			}
			else if(index>10){
				index=10;
			}
			return stack[index-1];
		}


};



#endif // CALC_PARSER_HPP
:/


/.


#include "calc_parser.hpp"
#include "calc_lexer.h"



#include <QtDebug>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////

bool CalcLexer::init(const char *in){
	buf=yy_scan_string(in);
	return 0!=buf;
}

bool CalcLexer::deinit(){
	buf=0;
	return true;
}

////////////////////////////////////////////////////////////////////////////

void CalcParser::consumeRule(int ruleno){
	qDebug()<<"RULE: "<<ruleno;
	/*
	switch (ruleno) {
		./

Goal: Expression ;
		/.
		case $rule_number:
			qDebug() << "value:" << sym(1);
			break;
			./

PrimaryExpression: number ;
PrimaryExpression: lparen Expression rparen ;
			/.
		case $rule_number:
			sym(1) = sym (2);
			break;
			./

Expression: PrimaryExpression ;

Expression: Expression plus PrimaryExpression;
			/.
		case $rule_number:
			sym(1) += sym (3);
			break;
			./

Expression: Expression minus PrimaryExpression;
			/.
		case $rule_number:
			sym(1) -= sym (3);
			break;
			./


Expression: Expression multiply PrimaryExpression;
			/.
		case $rule_number:
			sym(1) *= sym (3);
			break;
			./

Expression: Expression divide PrimaryExpression;
			/.
		case $rule_number:
			sym(1) /= sym (3);
			break;
			./



			/.

	} // switch
	*/
}


./


