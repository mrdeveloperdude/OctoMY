#include "TestWizardWidget.hpp"

#include "components/navigation/WizardWidget.hpp"
#include "uptime/New.hpp"

#include <QLabel>
#include <QSignalSpy>


QWidget *dummyPage(QString title){
	auto const page = new QLabel();
	page->setText(title);
	page->setObjectName(title);
	page->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	return page;
}

void TestWizardWidget::test()
{
	auto w = OC_NEW WizardWidget();
	w->registerPage(dummyPage("Dummy Page 1"));
	w->registerPage(dummyPage("Dummy Page 2"));
	w->setAttribute(Qt::WA_DeleteOnClose);
	w->show();
	QSignalSpy spy(w, &WizardWidget::destroyed);
	spy.wait(1000000);

}


OC_TEST_MAIN(test, TestWizardWidget)

