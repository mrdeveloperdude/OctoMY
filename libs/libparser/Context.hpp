#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include <QString>
#include <QDebug>



// Context simply means "location in input during parsing".
class Context {
	private:
		int m_line;
		int m_column;
		int m_token;
		QString m_translationUnit;
		bool m_valid;

	public:

		explicit Context();

		int line();
		int column();
		int token();
		QString translationUnit();
		void setTranslationUnit(QString tu);
		void incLine();
		void incColumn();
		void incToken();
		bool isValid();
		QString toString();
};
#endif // CONTEXT_HPP
