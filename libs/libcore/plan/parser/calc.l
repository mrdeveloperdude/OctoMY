%option noyywrap

%{
#include <cstdlib>
#include "calc_parser.hpp"
#include <QDebug>

#define YY_DECL int CalcParser::nextToken()
%}

hex [0-9a-fA-F]
dec [1-9][0-9]*
oct [0-7]
digit [0-9]

fract {digit}*\.{digit}+|{digit}+\.
exp [eE][+-]?{digit}+

hexfract {hex}*\.{hex}+|{hex}+\.
binexp [pP][+-]?{digit}+

icst ({dec}|0{oct}*|0[xX]{hex}+)

%%

[\n]					{ /*++context.line;*/ }
[ \t\r]+				{ /* skip */ }

"descriptor" { return calc_table::TOK_DESCRIPTOR; }
"plan" { return calc_table::TOK_PLAN; }
"("     { return calc_table::TOK_LPAREN; }
")"     { return calc_table::TOK_RPAREN; }
"["     { return calc_table::TOK_LBRACKET; }
"]"     { return calc_table::TOK_RBRACKET; }
"{"     { return calc_table::TOK_LBRACE; }
"}"     { return calc_table::TOK_RBRACE; }

\"[^\"\n]*\"  {
				 //yylval.string = yytext;
				 return calc_table::TOK_STRING;
			  }
\"[^\"\n]*$ {
				 qWarning()<<"Unterminated string";
				 //yylval.string = yytext;
				 return calc_table::TOK_STRING;
			 }
%%
