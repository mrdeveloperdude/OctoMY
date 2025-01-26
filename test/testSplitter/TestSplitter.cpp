#include "TestSplitter.hpp"

#include <QSplitter>
#include "splitter/SplitterWidget.hpp"
#include "test/Utility.hpp"
#include "uptime/New.hpp"

void TestSplitter::test()
{
	Q_INIT_RESOURCE(icons);
	const auto editor = OC_NEW SplitterWidget();
	editor->show();
	editor->setWindowTitle("Splitter test");
	editor->setMinimumSize(300, 200);
	test::utility::waitForUIEnd(editor);
}


OC_TEST_MAIN(test, TestSplitter)

