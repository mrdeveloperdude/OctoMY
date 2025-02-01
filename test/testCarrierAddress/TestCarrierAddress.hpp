#ifndef TESTCarrierAddress_HPP
#define TESTCarrierAddress_HPP

#include "test/Common.hpp"


class TestCarrierAddress:public QObject
{
	Q_OBJECT

public:
	explicit TestCarrierAddress();

private:
	const QHostAddress expectedEmptyAddress;
	const quint16 expectedEmptyPort;

	const QHostAddress expectedValidAddress;
	const quint16 expectedValidPort;

private slots:
	void initTestCase();
	void testInitial();
	// TODO: Fix after CarrierAddress refactor
	/*
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
	void testToFromString();
*/

};


#endif
// TESTCarrierAddress_HPP
