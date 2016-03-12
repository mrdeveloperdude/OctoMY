#ifndef GENERALPURPOSEPARSER_H
#define GENERALPURPOSEPARSER_H


#include <QString>
#include <QDebug>

class LexerBase{
	public:
		virtual void init(const char *);
		virtual void deinit();
};

//Base class that provides a pure Qt based interface by qlalr/flex based parsers.
template <typename Lexer, typename Table>
class GeneralPurposeParser{
	private:
		Lexer lexer;
		Table table;
	public:


		bool parse(QString);

	private:

		inline int nextToken(){
			return lexer.nextToken();
		}
};


template <typename Lexer, typename Table>
bool GeneralPurposeParser<Lexer, Table>::parse(QString s){
	bool status=lexer.init(s.toStdString().c_str());
	int t=0;
	do{
//		t=lexer.nextToken();
	//	qDebug()<<"TOK: "<<QString::number(t);
	}while(t>0);
	status &=lexer.deinit();
	return status;
}

#endif // GENERALPURPOSEPARSER_H

