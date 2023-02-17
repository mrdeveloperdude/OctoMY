#include "TestApp.hpp"

#include <cstdlib>
#include <QDebug>

int main(int argc, char *argv[])
{
	Q_UNUSED(argc);
	Q_UNUSED(argv);
	int ret=EXIT_FAILURE;
	qDebug()<<"TestApp ran";
	exit(ret);
}
