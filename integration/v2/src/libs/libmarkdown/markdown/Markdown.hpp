#ifndef MARKDOWN_HPP
#define MARKDOWN_HPP

#include <QString>

class Markdown
{

public:
	Markdown();
public:

	static bool isAvailable();

	QString process (QString input);

};

#endif // MARKDOWN_HPP
