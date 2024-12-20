#ifndef TEST_INCLUDED

#include "test.inc.hpp"

void testActuatorValueParserInclude(ArduMYActuatorSet &inSet)
{
#endif

	ArduMYActuatorSet outSet;
	const auto inSize=inSet.size();
	// The out set must match in size as this would be carried out by the set size command
	outSet.setSize(inSize);
	// We also copy the representations of the sets as that would have been carried out in a series of set config commands
	for(size_t i =0; i<inSize; ++i) {
		outSet[i].config.representation=inSet[i].config.representation;
	}
	Converter cv;
	ArduMYActuatorValueParser parser;

	// Check that it was default-initialized properly
	QCOMPARE(parser.set, (ArduMYActuatorSet *)nullptr);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::END_OF_OP);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)0);

	parser.setSet(outSet);

	// Check that the actuator set was received properly
	QCOMPARE(parser.set, &outSet);
	QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ENABLED_ACTUATOR_BITS);
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)0);

	// Reset enable bits to all off
	const size_t enableBitsSize=sizeof(parser.enableBits);
	uint8_t enableBits[enableBitsSize]= {0};
	for(size_t i=0; i<enableBitsSize; ++i) {
		enableBits[i]=0x0;
	}
	// Feed in enable bit sequence from input while counting on-bits
	uint8_t enabledCount=0;
	const uint8_t enableBitsActualSize = ( inSize + 7 ) / 8;
	for( size_t i = 0; i < enableBitsActualSize ; ++i ) {
		const uint8_t bits = ( utility::random::qrand() % 0xFF );
		for( size_t j = 0 ; j < 8 ; ++j ) {
			// Make sure to not enable bits outside the number of actuators in the set!
			if( ( i * 8 + j ) >= inSize ) {
				break;
			}
			const uint8_t mask = ( 1 << j );
			const uint8_t value = ( mask & bits );
			if( 0 != value ) {
				enableBits[i] |= value;
				enabledCount++;
			}
		}
		// qDebug()<<"RANDOM BITS: " << byteToStr(bits) << " ACTUALLY WRITTEN: " << byteToStr(enableBits[i]);
	}
	//qDebug()<<"SERIALIZER ACTUATOR COUNT: " << inSize << " ENABLE BITS BYTE COUNT: " << enableBitsActualSize << " ENABLED COUNT: " << enabledCount;
	// Parse enable bits
	for(size_t i=0; i<enableBitsActualSize; ++i) {
		QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ENABLED_ACTUATOR_BITS);
		QCOMPARE(parser.byteIndex, (uint16_t)i);
		parser.parse(enableBits[i]);
	}
	// Check that enabled bits were transferred correctly
	for(size_t i=0; i<enableBitsSize; ++i) {
		if(parser.enableBits[i]!=enableBits[i] ) {
			qDebug()<<"ENABLE-BIT MISMATCH DETECTED FOR BYTE "<< QString("%1").arg(i+1, 2, 10, QLatin1Char(' '))<<"/"<<enableBitsSize<<"  PARSER: "<<byteToStr(parser.enableBits[i])<<" vs. ORIGINAL: "<<byteToStr(enableBits[i]);
		}
		QCOMPARE(parser.enableBits[i], enableBits[i]);
	}
	// Check that parse step was advanced properly
	if(ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES != parser.step) {
		for(size_t i=0; i<enableBitsSize; ++i) {
			if(parser.enableBits[i]!=enableBits[i] ) {
				qDebug()<<"ENABLE-BIT MISMATCH DETECTED FOR BYTE "<< QString("%1").arg(i+1, 2, 10, QLatin1Char(' '))<<"/"<<enableBitsSize<<"  PARSER: "<<byteToStr(parser.enableBits[i])<<" vs. ORIGINAL: "<<byteToStr(enableBits[i]);
			}
		}
	}
	QCOMPARE(parser.byteIndex, (uint16_t)0);
	QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
	if(enabledCount>0) {
		QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
	} else {
		QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::END_OF_OP);
	}


	// Send values and check that they were sucessfully transferred per batch
	QString acc;
	uint8_t bitCount=0;
	uint8_t bits=0x00;
	uint8_t byteIndex=0;

	///// BIT ACTUATOR VALUES
	////////////////////////////
	// Bit actuator values are accumulated into bytes before sending
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_BIT==a.config.representation) {
				if(a.state.value.bit) {
					bits |= ( 1 << bitCount );
				}
				bitCount++;
				if( bitCount >= 8 ) {
					acc += byteToStr(bits)+" ";
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
//					QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)byteIndex);
					byteIndex++;
					parser.parse(bits);
					bitCount=0;
					bits=0x00;
				}
			}
		}
	}
	// Send any remaining bits we have
	if(bitCount>0) {
		//acc+=" REST: "+byteToStr(bits);
		QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)ArduMYActuatorValueRepresentation::VALREP_BIT);
		QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)0);//One would think that byteIndex was the same BUT it isn't so get over it!
		byteIndex++;
		parser.parse(bits);
		bitCount=0;
		bits=0;
	}

	//qDebug()<<"ACCUMULATED BIT VALUES SENT:       "<<acc;

	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_BIT == a.config.representation) {
				//qDebug()<<"SENT BIT VALUE FOR "<<byte<<"."<<bit<<": " << a.state.value.bit;
				ArduMYActuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.bit, b.state.value.bit);
			}
		}
	}

	///// BYTE ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_BYTE==a.config.representation) {
				//qDebug().noquote().nospace()<<"SENDING BYTE VALUE FOR "<<byte<<"."<<bit<<" (" << QString("%1").arg(i,2,10,QChar(' '))<< "/" << QString("%1").arg(inSize,2,10,QChar(' '))<< "): "<<a.state.value.byte;
				QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
				QCOMPARE(parser.currentActuatorIndex, (int16_t )i);
				ArduMYActuator &b=outSet[i];
				QCOMPARE((uint8_t)b.config.representation, (uint8_t)a.config.representation);
				parser.parse(a.state.value.byte);
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.byte, b.state.value.byte);
			}
		}
	}

///// WORD ACTUATOR VALUES
////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_WORD==a.config.representation) {
				//qDebug()<<"SENDING WORD VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.word;
				cv.int64=0x00;
				cv.uint16[0]=a.state.value.word;
				for(int j=0; j<2; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
					QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)j);
					QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
					parser.parse(cv.uint8[j]);
				}
				ArduMYActuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.word, b.state.value.word);
			}
		}
	}

	///// DWORD ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD==a.config.representation) {
				//qDebug()<<"SENDING DWORD VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.doubleWord;
				cv.int64=0x00;
				cv.uint32[0]=a.state.value.doubleWord;
				for(int j=0; j<4; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
					QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)j);
					QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
					parser.parse(cv.uint8[j]);
				}
				ArduMYActuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.doubleWord, b.state.value.doubleWord);
			}
		}
	}

	///// QWORD ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD==a.config.representation) {
				//qDebug()<<"SENDING QWORD VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.quadWord;
				cv.int64=0x00;
				cv.uint64=a.state.value.quadWord;
				for(int j=0; j<8; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
					QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)j);
					QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
					parser.parse(cv.uint8[j]);
				}
				ArduMYActuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.quadWord, b.state.value.quadWord);
			}
		}
	}

	///// SINGLE ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT==a.config.representation) {
				//qDebug()<<"SENDING FLOAT VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.singlePrecision;
				cv.int64=0x00;
				cv.float32[0]=a.state.value.singlePrecision;
				for(int j=0; j<4; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
					QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)j);
					QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
					parser.parse(cv.uint8[j]);
				}
				ArduMYActuator &b=outSet[i];
				QCOMPARE(a.config.representation, b.config.representation);
				QCOMPARE(a.state.value.singlePrecision, b.state.value.singlePrecision);
			}
		}
	}

	///// DOUBLE ACTUATOR VALUES
	////////////////////////////
	for(size_t i=0; i<inSize; ++i) {
		const uint8_t byte=(i / 8);
		const uint8_t bit= (i % 8);
		const uint8_t mask=( 1 << bit );
		if( 0 != ( enableBits[byte] & mask ) ) {
			ArduMYActuator &a=inSet[i];
			if(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT==a.config.representation) {
				ArduMYActuator &b=outSet[i];
				//qDebug()<<"Actuator:"<<i<<", value: "<<a.state.value.doublePrecision<<", mask="<<mask;
				QCOMPARE(a.config.representation, b.config.representation);
				//qDebug()<<"SENDING DOUBLE VALUE FOR "<<byte<<"."<<bit<<": "<<a.state.value.doublePrecision;
				cv.int64=0x00;
				cv.float64=a.state.value.doublePrecision;
				//QCOMPARE(sizeof(a.state.value.doublePrecision), (size_t)8);
				for(int j=0; j<8; ++j) {
					QCOMPARE((uint8_t)parser.currentBatchRepresentation, (uint8_t)a.config.representation);
					QCOMPARE(parser.enabledActuatorCount, (uint8_t)enabledCount);
					QCOMPARE((uint8_t)parser.byteIndex, (uint8_t)j);
					//qDebug()<<" + ByteIndex/J:"<<parser.byteIndex<<", J="<<j;
					QCOMPARE((uint8_t)parser.step, (uint8_t)ArduMYActuatorValuesParserStep::ACTUATOR_VALUE_BATCHES);
					parser.parse(cv.uint8[j]);
				}

				QCOMPARE(a.state.value.doublePrecision, b.state.value.doublePrecision);
			}
		}
	}


#ifndef TEST_INCLUDED
}
#endif































































































































