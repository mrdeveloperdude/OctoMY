#ifndef PARSEERROR_HPP
#define PARSEERROR_HPP

#include "Context.hpp"
#include <QString>

struct ParseError{
	Context context;
	QString message;
	QString hint;

	ParseError(Context context=Context(), QString message="", QString hint="")
	: context(context)
	, message(message)
	, hint(hint)
	{
	}

	QString toString(){
		QString out=message;
		if(context.isValid()){
			out+=" @ "+context.toString();
		}
		return out;
	}
};


#endif // PARSEERROR_HPP
