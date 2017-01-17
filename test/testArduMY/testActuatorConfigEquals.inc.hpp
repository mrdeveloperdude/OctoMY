#ifndef TEST_INCLUDED

#include "../arduino/ActuatorConfig.hpp"


#include "../arduino/ActuatorConfigParser.hpp"
#include "../arduino/ActuatorConfigSerializer.hpp"


#include "../arduino/ActuatorValueParser.hpp"
#include "../arduino/ActuatorValueSerializer.hpp"

#include "../arduino/MagicDetector.hpp"


#include "../arduino/CommandParser.hpp"

#include "../arduino/ParserState.hpp"

#include <QTest>

void testActuatorConfigEqualsInclude(ActuatorConfig &a)
{
#endif


	QCOMPARE(a,a);
	QVERIFY(a.isEqual(a));
	QVERIFY(a==a);
	for(int i=0; i<8; ++i) {
		ActuatorConfig b=a;
		QCOMPARE(a,b);
		QVERIFY(a.isEqual(b));
		QVERIFY(b.isEqual(a));
		QVERIFY(a==b);

		switch(i) {
		case(0): {
			b.setAbsoluteEncoder(!a.hasAbsoluteEncoder());
		}
		break;
		case(1): {
			b.setContinuous(!a.isContinuous());
		}
		break;
		case(2): {
			//b.setDirty(!a.isDirty());
			b.setGearRatio(!a.hasGearRatio());
		}
		break;
		case(3): {
			b.setIncrementalEncoder(!a.hasIncrementalEncoder());
		}
		break;
		case(4): {
			b.setLimitSwitchEnd(!a.hasLimitSwitchEnd());
		}
		break;
		case(5): {
			b.setLimitSwitchStart(!a.hasLimitSwitchStart());
		}
		break;
		case(6): {
			b.setLinear(!a.isLinear());
		}
		break;
		case(7): {
			b.setPositionFeedback(!a.hasPositionFeedback());
		}
		break;
		case(8): {
			b.setTachometer(!a.hasTachometer());
		}
		break;
			//TODO: Add cases to exercise the rest of equals code (not just for flags).
		}
		QVERIFY(!a.isEqual(b));
		QVERIFY(!b.isEqual(a));
		QVERIFY(a!=b);
	}




#ifndef TEST_INCLUDED

}
#endif
