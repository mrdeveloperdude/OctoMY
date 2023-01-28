#include "TestMarkdownEditor.hpp"

#include "widgets/markdown/MarkdownEditor.hpp"
#include "utility/random/Random.hpp"
#include "Utility_test.hpp"
#include "uptime/ConnectionType.hpp"
#include "MarkdownTestInput.hpp"


void TestMarkdownEditor::test()
{
	MarkdownEditor w, *wp=&w;
	w.setText(markdownTestInput);
	w.show();
	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=]() {
	}, OC_CONTYPE_NON_UNIQUE);
	test::utility::waitForUIEnd(wp);
}


OC_TEST_MAIN(test, TestMarkdownEditor)

