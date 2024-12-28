#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <QString>

class Response {
public:
	const bool success = true;
	const QString message;
	const int line = -1;
	const int column = -1;
public:
	Response(const QString &message, const int line = -1, const int column = -1)
		: success{message.trimmed().isEmpty()}
		, message{message.trimmed()}
		, line{line}
	, column{column}
	{
		
	};
	Response()
		: success{true}
	{
	};
};

#endif // RESPONSE_HPP
