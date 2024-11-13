#include "TestEditableLabel.hpp"

#include "components/EditableLabel.hpp"
#include "uptime/New.hpp"
#include "test/Utility.hpp"

void TestEditableLabel::test()
{
	Q_INIT_RESOURCE(icons);
	auto el = OC_NEW EditableLabel();
	el->show();
	el->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter );
	test::utility::waitForUIEnd(el);
}


OC_TEST_MAIN(test, TestEditableLabel)

