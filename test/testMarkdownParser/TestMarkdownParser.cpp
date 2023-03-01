#include "TestMarkdownParser.hpp"

#include "markdown/Markdown.hpp"


#include <QDebug>

void TestMarkdownParser::test()
{
	
	Markdown md;
	QString input = R"END(
# Heading
## Subheading
body, *italic*, **bold** and stuff!
)END";
	QString expected = R"END(<h1>Heading</h1>

<h2>Subheading</h2>

<p>body, <em>italic</em>, <strong>bold</strong> and stuff!</p>
)END";
	auto output = md.process(input);
	
	qWarning()<<"IN:\n"<<input;
	qWarning()<<"EXPECTED:\n"<<expected;
	qWarning()<<"OUT:\n"<<output;
	QCOMPARE(expected, output);
}


OC_TEST_MAIN(test, TestMarkdownParser)

