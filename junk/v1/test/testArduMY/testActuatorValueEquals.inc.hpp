#ifndef TEST_INCLUDED

#include "test.inc.hpp"

void testActuatorValueEqualsInclude(ArduMYActuatorSet &a)
{
#endif

	QCOMPARE(a,a);
	QVERIFY(a.isEqual(a));
	QVERIFY(a==a);
	const auto setSize=a.size();
	for(size_t i=0; i<setSize; ++i) {


		for(int j=0; j<8; ++j) {
			ArduMYActuatorSet b=a;
			QCOMPARE(a,b);
			QVERIFY(a.isEqual(b));
			QVERIFY(b.isEqual(a));
			QVERIFY(a==b);

			switch(j) {
			case(0): {
				b[i].config.setAbsoluteEncoder(!a[i].config.hasAbsoluteEncoder());
			}
			break;
			case(1): {
				b[i].config.setContinuous(!a[i].config.isContinuous());
			}
			break;
			case(2): {
				//b.setDirty(!a.isDirty());
				b[i].config.setGearRatio(!a[i].config.hasGearRatio());
			}
			break;
			case(3): {
				b[i].config.setIncrementalEncoder(!a[i].config.hasIncrementalEncoder());
			}
			break;
			case(4): {
				b[i].config.setLimitSwitchEnd(!a[i].config.hasLimitSwitchEnd());
			}
			break;
			case(5): {
				b[i].config.setLimitSwitchStart(!a[i].config.hasLimitSwitchStart());
			}
			break;
			case(6): {
				b[i].config.setLinear(!a[i].config.isLinear());
			}
			break;
			case(7): {
				b[i].config.setPositionFeedback(!a[i].config.hasPositionFeedback());
			}
			break;
			case(8): {
				b[i].config.setTachometer(!a[i].config.hasTachometer());
			}
			break;
				//TODO: Add cases to exercise the rest of equals code (not just for flags).
			}
			QVERIFY(!a.isEqual(b));
			QVERIFY(!b.isEqual(a));
			QVERIFY(a!=b);
		}
	}

#ifndef TEST_INCLUDED
}
#endif
