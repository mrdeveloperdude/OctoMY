#include "TestParser.hpp"


#ifdef USE_PARSER
#include "octomy_parser.hpp"
#endif


// YOU NEED THIS: http://doc.qt.io/qt-5/qtest.html
void TestParser::test()
{
#ifdef USE_PARSER
	OctomyParser p;
	p.parse("plan{ remote{} hub {} agent{puppy{ memeber { } } } }");
	qDebug()<<"PARSE RESULT: "<<p.treesToString();
#endif
}



OC_TEST_MAIN(test, TestParser)
