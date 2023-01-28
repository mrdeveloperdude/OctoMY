#include "TestAppLauncher.hpp"

#include <QSharedPointer>

#include "app/launcher/AppLauncher.hpp"
#include "app/mockapp/MockApp.hpp"

#include "Common_test.hpp"

void TestAppLauncher::test()
{
	int argc = 1;
	char mockExeName[] = "mockExecutableName";
	char* argv[] = { static_cast <char *>(mockExeName) };
	QSharedPointer<AppLauncher<MockApp> > mockMain=QSharedPointer<AppLauncher<MockApp> >(OC_NEW AppLauncher<MockApp>());
	Q_ASSERT(!mockMain.isNull());
	mockMain->configure("mock", argc, argv);
	auto ret=mockMain->run();
	mockMain.clear();
	qDebug()<<"Test ended with"<<ret;
}




OC_TEST_MAIN(test, TestAppLauncher)

