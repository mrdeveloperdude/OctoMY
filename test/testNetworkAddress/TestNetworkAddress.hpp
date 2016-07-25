#ifndef TESTNETWORKADDRESS_HPP
#define TESTNETWORKADDRESS_HPP

#include <QTest>


class TestNetworkAddress:public QObject{
		Q_OBJECT
	public:
		explicit TestNetworkAddress();
	private:

		const QHostAddress expectedEmptyAddress;
		const quint16 expectedEmptyPort;

		const QHostAddress expectedValidAddress;
		const quint16 expectedValidPort;


	private slots:
		void initTestCase();
		void testInitial();
		void testWithoutConstructorArguments();
		void testEmptyMapConstructorArguments();
		void testInvalidMapConstructorArguments();
		void testValidMapConstructorArguments();
		void testEmptyDirectConstructorArguments();
		void testInvalidDirectConstructorArguments();
		void testValidDirectConstructorArguments();
		void testAssignment1();
		void testAssignment2();
		void testIsValid();

};


#endif // TESTNETWORKADDRESS_HPP
