#include "TestMarkdownEditor.hpp"

#include "markdown/MarkdownEditor.hpp"
#include "test/Utility.hpp"
#include "uptime/ConnectionType.hpp"
#include "MarkdownTestInput.hpp"


void TestMarkdownEditor::test()
{
	markdown::MarkdownEditor w, *wp=&w;
	w.setText(markdownTestInput);
	w.show();
	QTimer t;
	t.start(10);
	connect(&t,&QTimer::timeout, this, [=]() {
	}, OC_CONTYPE_NON_UNIQUE);
	test::utility::waitForUIEnd(wp);
}


OC_TEST_MAIN(test, TestMarkdownEditor)

