#include "TestFork.hpp"

#include "test/Common.hpp"

#include <QDebug>
#include <QApplication>
#include <QPushButton>
#include <QTimer>
#include <QAbstractEventDispatcher>
#include <QOpenGLContext>
#include <QtDBus/QDBusConnection>

#include <csignal>
#include <unistd.h> // For fork
#include <sys/wait.h> // For waitpid


void TestFork::test()
{
	qDebug() << "TEST FORK";
	pid_t pid = fork();
	
	if (pid < 0)
	{
		qCritical() << "Fork failed!";
		return;
	}
	else if (pid == 0)
	{
		qDebug() << "In child process A1a";
		
		QDBusConnection::disconnectFromBus("session");
		qDebug() << "In child process A1b";
		QDBusConnection::disconnectFromBus("system");

		// Reset OpenGL contexts if they exist
		if (QOpenGLContext::currentContext()) {
			qDebug() << "In child process A6a";
			QOpenGLContext::currentContext()->doneCurrent();
			qDebug() << "In child process A6b";
			QOpenGLContext::currentContext()->deleteLater();
		}
		
		
		qDebug() << "In child process A7a";
		// Reset signal handlers to default
		signal(SIGPIPE, SIG_DFL);
		qDebug() << "In child process A7b";
		signal(SIGCHLD, SIG_DFL);
		qDebug() << "In child process A8";
		
		/*
		close(0); // Close standard input
		close(1); // Close standard output
		close(2); // Close standard error
*/
		qDebug() << "In child process A9";
		for (int fd = 3; fd < 1024; ++fd) {
			close(fd);
		}
		
		if (QAbstractEventDispatcher::instance() != nullptr)
		{
			qDebug() << "An existing event loop is running, exiting it.";
			QCoreApplication::exit();  // End the existing event loop if running
		}
		
		
		int argc = 0;
		char **argv = nullptr;
		qDebug() << "In child process B1";
		auto childApp = new QApplication (argc, argv);
		qDebug() << "In child process B2";
		QPushButton *button = new QPushButton("Terminate Child");
		qDebug() << "In child process C";
		
		
		QObject::connect(button, &QPushButton::clicked, childApp, &QApplication::quit);
		qDebug() << "In child process D";
		
		QTimer::singleShot(100, [button]() {
			qDebug() << "In child process E";
			button->show();
			qDebug() << "In child process F";
		});
		
		qDebug() << "In child process G";
		childApp->exec();
		qDebug() << "In child process H";
		delete childApp;
		qDebug() << "In child process I";
		childApp = nullptr;
		qDebug() << "In child process J";
		
		_exit(0);  // Ensure clean exit of the child process
	}
	else
	{
		qDebug() << "In parent process";
		int status;
		waitpid(pid, &status, 0);
	}
}

int main(int argc, char *argv[]){
	//QApplication::setAttribute(Qt::AA_Use96Dpi, true);
	//const auto app = new QApplication (argc, argv);
	TestFork test;
	test.test();
}





//OC_TEST_MAIN(test, TestFork)

