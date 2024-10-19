#ifndef TESTAGENTCONFIG_HPP
#define TESTAGENTCONFIG_HPP

#include "test/Common.hpp"

class TestAgentConfig:public QObject
{
	Q_OBJECT
private slots:
	void testAgentConfig();
	void testAgentConfigStore();

};



#endif
// TESTAGENTCONFIG_HPP
