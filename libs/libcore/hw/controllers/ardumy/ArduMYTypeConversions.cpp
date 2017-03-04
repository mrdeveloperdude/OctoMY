#include "ArduMYTypeConversions.hpp"

#include "../arduino/ArduMYActuatorState.hpp"
#include "../arduino/ArduMYActuatorConfig.hpp"
#include "../arduino/ArduMYActuator.hpp"

#include "../arduino/ArduMYActuatorSet.hpp"
#include "../arduino/ArduMYCommandSerializer.hpp"
#include "../arduino/ArduMYCommandParser.hpp"


/*




QString ArduMYActuatorValueRepresentationToString(ArduMYActuatorValueRepresentation s)
{
#define  ArduMYActuatorTypeToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ArduMYActuatorTypeToStringCASE(BIT);
		ArduMYActuatorTypeToStringCASE(BYTE);
		ArduMYActuatorTypeToStringCASE(WORD);
		ArduMYActuatorTypeToStringCASE(DOUBLE_WORD);
		ArduMYActuatorTypeToStringCASE(QUAD_WORD);
		ArduMYActuatorTypeToStringCASE(SINGLE_FLOAT);
		ArduMYActuatorTypeToStringCASE(DOUBLE_FLOAT);
		ArduMYActuatorTypeToStringCASE(REPRESENTATION_COUNT);

	}
#undef ArduMYActuatorTypeToStringCASE
}




QString ArduMYActuatorValueToString(ArduMYActuatorValue val, ArduMYActuatorValueRepresentation representation)
{
	switch(representation) {
	case(ArduMYActuatorValueRepresentation::BIT): {
		return val.bit?"true":"false";
	}
	break;
	case(ArduMYActuatorValueRepresentation::BYTE): {
		return QString::number((quint8)val.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::WORD): {
		return QString::number((quint16)val.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
		return QString::number((quint32)val.doubleWord);
	}
	break;
	//Default to strictest possible equality when representation is unknown
	default:
	case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT):
	case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
		return QString::number((quint64)val.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
		return QString::number((float)val.singlePrecision);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
		return QString::number((double)val.doublePrecision);
	}
	break;
	}
	return "UNKNOWN";
}



QString ArduMYActuatorStateToString(ArduMYActuatorState &state, ArduMYActuatorValueRepresentation representation)
{
	QString out="ArdumyActuatorState{ flags="+QString::number(state.flags,2)+", value="+ArduMYActuatorValueToString(state.value, representation)+ " }";
	return out;
}

QString ArduMYActuatorConfigToString(ArduMYActuatorConfig &config)
{
	QString out;
	return out;
}



*/

const ArduMYActuatorValueRepresentation DEFAULT_REPRESENTATION=REPRESENTATION_COUNT;



/////////////////////////////////////////////



const ArduMYActuatorValuesParserStep DEFAULT_VALUE_PARSER_STEP=END_OF_OP;



QString ardumyActuatorValueParserStepToString(const ArduMYActuatorValuesParserStep &s)
{
#define  ardumyArduMYActuatorValueParserStepToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ardumyArduMYActuatorValueParserStepToStringCASE(ENABLED_ACTUATOR_BITS);
		ardumyArduMYActuatorValueParserStepToStringCASE(ACTUATOR_VALUE_BATCHES);
		ardumyArduMYActuatorValueParserStepToStringCASE(END_OF_OP);
	}
#undef ardumyArduMYActuatorValueParserStepToStringCASE
}


ArduMYActuatorValuesParserStep ardumyActuatorValueParserStepFromString(const QString &s)
{
#define  ardumyArduMYActuatorValueParserStepFromStringCASE(A)   if(#A==s) { return A; }
	ardumyArduMYActuatorValueParserStepFromStringCASE(ENABLED_ACTUATOR_BITS);
	ardumyArduMYActuatorValueParserStepFromStringCASE(ACTUATOR_VALUE_BATCHES);
	ardumyArduMYActuatorValueParserStepFromStringCASE(END_OF_OP);
#undef ardumyArduMYActuatorValueParserStepFromStringCASE
	return DEFAULT_VALUE_PARSER_STEP;
}





/////////////////////////////////////////////



const ArduMYActuatorConfigParserStep DEFAULT_CONFIG_PARSER_STEP=END_OF_OPERATION;



QString ardumyActuatorConfigParserStepToString(const ArduMYActuatorConfigParserStep &s)
{
#define  ardumyArduMYActuatorConfigParserStepToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ardumyArduMYActuatorConfigParserStepToStringCASE(FLAGS);
		ardumyArduMYActuatorConfigParserStepToStringCASE(TYPE);
		ardumyArduMYActuatorConfigParserStepToStringCASE(REPRESENTATION);
		ardumyArduMYActuatorConfigParserStepToStringCASE(NICK);
		ardumyArduMYActuatorConfigParserStepToStringCASE(GEAR_NUMERATOR);
		ardumyArduMYActuatorConfigParserStepToStringCASE(GEAR_DENOMINATOR);
		ardumyArduMYActuatorConfigParserStepToStringCASE(POSITION_FEEDBACK);
		ardumyArduMYActuatorConfigParserStepToStringCASE(TACHOMETER);
		ardumyArduMYActuatorConfigParserStepToStringCASE(ENCODER_PIN_A);
		ardumyArduMYActuatorConfigParserStepToStringCASE(ENCODER_PIN_B);
		ardumyArduMYActuatorConfigParserStepToStringCASE(ENCODER_DEBOUNCE);
		ardumyArduMYActuatorConfigParserStepToStringCASE(LIMIT_SWITCH_PIN_START);
		ardumyArduMYActuatorConfigParserStepToStringCASE(LIMIT_SWITCH_PIN_END);
		ardumyArduMYActuatorConfigParserStepToStringCASE(LIMIT_SWITCH_DEBOUNCE);
		ardumyArduMYActuatorConfigParserStepToStringCASE(STEP_PHASE_COUNT);
		ardumyArduMYActuatorConfigParserStepToStringCASE(STEP_STEPS_PER_ROTATION);
		ardumyArduMYActuatorConfigParserStepToStringCASE(RC_SERVO_PIN);
		ardumyArduMYActuatorConfigParserStepToStringCASE(RANGE_START);
		ardumyArduMYActuatorConfigParserStepToStringCASE(RANGE_SPAN);
		ardumyArduMYActuatorConfigParserStepToStringCASE(END_OF_OPERATION);
	}
#undef ardumyArduMYActuatorConfigParserStepToStringCASE
}


ArduMYActuatorConfigParserStep ardumyActuatorConfigParserStepFromString(const QString &s)
{
#define  ardumyArduMYActuatorConfigParserStepFromStringCASE(A)   if(#A==s) { return A; }
	ardumyArduMYActuatorConfigParserStepFromStringCASE(FLAGS);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(TYPE);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(REPRESENTATION);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(NICK);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(GEAR_NUMERATOR);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(GEAR_DENOMINATOR);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(POSITION_FEEDBACK);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(TACHOMETER);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(ENCODER_PIN_A);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(ENCODER_PIN_B);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(ENCODER_DEBOUNCE);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(LIMIT_SWITCH_PIN_START);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(LIMIT_SWITCH_PIN_END);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(LIMIT_SWITCH_DEBOUNCE);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(STEP_PHASE_COUNT);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(STEP_STEPS_PER_ROTATION);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(RC_SERVO_PIN);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(RANGE_START);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(RANGE_SPAN);
	ardumyArduMYActuatorConfigParserStepFromStringCASE(END_OF_OPERATION);
#undef ardumyArduMYActuatorConfigParserStepFromStringCASE
	return DEFAULT_CONFIG_PARSER_STEP;
}




/////////////////////////////////////////////

const ArduMYActuatorType DEFAULT_ACTUATOR_TYPE=TYPE_COUNT;


QString ardumyActuatorTypeToString(const ArduMYActuatorType &s)
{
#define  ardumyActuatorTypeToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ardumyActuatorTypeToStringCASE(DC_MOTOR);
		ardumyActuatorTypeToStringCASE(STEP_MOTOR);
		ardumyActuatorTypeToStringCASE(RC_SERVO);
		ardumyActuatorTypeToStringCASE(RELAY);
		ardumyActuatorTypeToStringCASE(TYPE_COUNT);
	}
#undef ardumyActuatorTypeToStringCASE
}


ArduMYActuatorType ardumyActuatorTypeFromString(const QString &s)
{
#define  ardumyArduMYActuatorTypeFromStringCASE(A)   if(#A==s) { return A; }
	ardumyArduMYActuatorTypeFromStringCASE(DC_MOTOR);
	ardumyArduMYActuatorTypeFromStringCASE(STEP_MOTOR);
	ardumyArduMYActuatorTypeFromStringCASE(RC_SERVO);
	ardumyArduMYActuatorTypeFromStringCASE(RELAY);
#undef ardumyArduMYActuatorTypeFromStringCASE
	return DEFAULT_ACTUATOR_TYPE;
}

/////////////////////////////////////////////


const ArduMYParserState DEFAULT_PARSER_STATE=OCTOMY_COUNT;


QString ardumyParserStateToString(const ArduMYParserState &s)
{
#define  ardumyParserStateToStringCASE(A) case (A):return #A
	switch(s) {
	default:
		return "UNKNOWN";
		ardumyParserStateToStringCASE(OCTOMY_SYNC);
		ardumyParserStateToStringCASE(OCTOMY_AWAITING_COMMAND);
		ardumyParserStateToStringCASE(OCTOMY_GET_STATUS);
		ardumyParserStateToStringCASE(OCTOMY_SET_ACTUATOR_COUNT);
		ardumyParserStateToStringCASE(OCTOMY_SET_ACTUATOR_CONFIG);
		ardumyParserStateToStringCASE(OCTOMY_SET_ACTUATOR_VALUES);
		ardumyParserStateToStringCASE(OCTOMY_SET_ACTUATOR_LIMP);
		ardumyParserStateToStringCASE(OCTOMY_UNKNOWN);
		ardumyParserStateToStringCASE(OCTOMY_COUNT);
	}
#undef ardumyParserStateToStringCASE
}


ArduMYParserState ardumyParserStateFromString(const QString &s)
{
#define  ardumyParserStateFromStringCASE(A)   if(#A==s) { return A; }
	ardumyParserStateFromStringCASE(OCTOMY_SYNC);
	ardumyParserStateFromStringCASE(OCTOMY_AWAITING_COMMAND);
	ardumyParserStateFromStringCASE(OCTOMY_GET_STATUS);
	ardumyParserStateFromStringCASE(OCTOMY_SET_ACTUATOR_COUNT);
	ardumyParserStateFromStringCASE(OCTOMY_SET_ACTUATOR_CONFIG);
	ardumyParserStateFromStringCASE(OCTOMY_SET_ACTUATOR_VALUES);
	ardumyParserStateFromStringCASE(OCTOMY_SET_ACTUATOR_LIMP);
	ardumyParserStateFromStringCASE(OCTOMY_UNKNOWN);
	ardumyParserStateFromStringCASE(OCTOMY_COUNT);
#undef ardumyParserStateFromStringCASE
	return DEFAULT_PARSER_STATE;
}

/////////////////////////////////////////////



QString ardumyParserStateToString(const ArduMYParserState &ps);

/////////////////////////////////////////////

void ardumyActuatorValueFromVariant(ArduMYActuatorValue &val, const QVariant &variant, const ArduMYActuatorValueRepresentation &representation)
{
	switch(representation) {
	case(ArduMYActuatorValueRepresentation::BIT): {
		val.bit=variant.toBool();
	}
	break;
	case(ArduMYActuatorValueRepresentation::BYTE): {
		val.byte=variant.toUInt();
	}
	break;
	case(ArduMYActuatorValueRepresentation::WORD): {
		val.word=variant.toUInt();
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
		val.doubleWord=variant.toUInt();
	}
	break;
	//Default to strictest possible equality when representation is unknown
	default:
	case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT):
	case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
		val.quadWord=variant.toULongLong();
	}
	break;
	case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
		val.singlePrecision=variant.toFloat();
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
		val.doublePrecision=variant.toDouble();
	}
	break;
	}
}


QVariant ardumyActuatorValueToVariant(const ArduMYActuatorValue &val, const ArduMYActuatorValueRepresentation &representation)
{
	switch(representation) {
	case(ArduMYActuatorValueRepresentation::BIT): {
		return QVariant((bool)val.bit);
	}
	break;
	case(ArduMYActuatorValueRepresentation::BYTE): {
		return QVariant((quint8)val.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::WORD): {
		return QVariant((quint16)val.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_WORD): {
		return QVariant((quint32)val.doubleWord);
	}
	break;
	//Default to strictest possible equality when representation is unknown
	default:
	case(ArduMYActuatorValueRepresentation::REPRESENTATION_COUNT):
	case(ArduMYActuatorValueRepresentation::QUAD_WORD): {
		return QVariant((quint64)val.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::SINGLE_FLOAT): {
		return QVariant((float)val.singlePrecision);
	}
	break;
	case(ArduMYActuatorValueRepresentation::DOUBLE_FLOAT): {
		return QVariant((double)val.doublePrecision);
	}
	break;
	}
	return QVariant();
}




ArduMYActuatorValueRepresentation ardumyActuatorValueRepresentationFromString(const QString &s)
{
#define  ardumyActuatorTypeFromStringCASE(A) if (#A == s)return A
	ardumyActuatorTypeFromStringCASE(BIT);
	ardumyActuatorTypeFromStringCASE(BYTE);
	ardumyActuatorTypeFromStringCASE(WORD);
	ardumyActuatorTypeFromStringCASE(DOUBLE_WORD);
	ardumyActuatorTypeFromStringCASE(QUAD_WORD);
	ardumyActuatorTypeFromStringCASE(SINGLE_FLOAT);
	ardumyActuatorTypeFromStringCASE(DOUBLE_FLOAT);
	ardumyActuatorTypeFromStringCASE(REPRESENTATION_COUNT);

#undef ardumyActuatorTypeFromStringCASE
	return REPRESENTATION_COUNT;
}




QString ardumyActuatorValueRepresentationToString(const ArduMYActuatorValueRepresentation &rep)
{
#define  ArduMYActuatorTypeToStringCASE(A) case (A):return #A
	switch(rep) {
	default:
		return "UNKNOWN";
		ArduMYActuatorTypeToStringCASE(BIT);
		ArduMYActuatorTypeToStringCASE(BYTE);
		ArduMYActuatorTypeToStringCASE(WORD);
		ArduMYActuatorTypeToStringCASE(DOUBLE_WORD);
		ArduMYActuatorTypeToStringCASE(QUAD_WORD);
		ArduMYActuatorTypeToStringCASE(SINGLE_FLOAT);
		ArduMYActuatorTypeToStringCASE(DOUBLE_FLOAT);
		ArduMYActuatorTypeToStringCASE(REPRESENTATION_COUNT);

	}
#undef ArduMYActuatorTypeToStringCASE
}





QString ardumyActuatorValueToString(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep)
{
	QString ret=ardumyActuatorValueRepresentationToString(rep)+" ( ";
	switch(rep) {
	case(BIT):
		ret+=(v.bit?"TRUE":"FALSE");
		break;
	case(BYTE):
		ret+=QString::number(v.byte);
		break;
	case(WORD):
		ret+=QString::number(v.word);
		break;
	case(DOUBLE_WORD):
		ret+=QString::number(v.doubleWord);
		break;
	case(REPRESENTATION_COUNT):
		ret+="REPRESENTATION_COUNT";
		break;
	default:
		ret+="OUT_OF_RANGE!";
		break;
	case(QUAD_WORD): {
		ret+=QString::number(v.quadWord);
	}
	break;
	case(SINGLE_FLOAT):
		ret+=QString::number(v.singlePrecision);
		break;
	case(DOUBLE_FLOAT):
		ret+=QString::number(v.doublePrecision);
		break;
	}
	return ret+" )";
}


QString ardumyActuatorConfigFlagsToString(const ArduMYActuatorConfig &c)
{
	QStringList list;
	list<< (c.isDirty()?              QStringLiteral("dirty"): QStringLiteral("-----"));
	list<< (c.isContinuous()?         QStringLiteral("contn"): QStringLiteral("-----"));
	list<< (c.isLinear()?             QStringLiteral("liner"): QStringLiteral("-----"));
	list<< (c.hasGearRatio()?         QStringLiteral("gearr"): QStringLiteral("-----"));
	list<< (c.hasAbsoluteEncoder()?   QStringLiteral("abenc"): QStringLiteral("-----"));
	list<< (c.hasIncrementalEncoder()?QStringLiteral("inenc"): QStringLiteral("-----"));
	list<< (c.hasPositionFeedback()?  QStringLiteral("posfb"): QStringLiteral("-----"));
	list<< (c.hasTachometer()?        QStringLiteral("tacho"): QStringLiteral("-----"));
	list<< (c.hasLimitSwitchStart()?  QStringLiteral("start"): QStringLiteral("-----"));
	list<< (c.hasLimitSwitchEnd()?    QStringLiteral("endli"): QStringLiteral("-----"));

	return list.join(", ");
}

QString ardumyActuatorConfigToString(const ArduMYActuatorConfig &c)
{
	QString ret;
	ret+=(
			 //QString(" flags=%1").arg(c.flags,8,2,QChar('0'))
			 ardumyActuatorConfigFlagsToString(c)
			 +QStringLiteral("; rep=")
			 +ardumyActuatorValueRepresentationToString(c.representation)
		 );
	return ret;
}


QString ardumyActuatorStateFlagsToString(const ArduMYActuatorState &s)
{
	QStringList list;
	list<< (s.isDirty()?              QStringLiteral("dirty"): QStringLiteral("-----"));
	list<< (s.isLimp()?               QStringLiteral("limpp"): QStringLiteral("-----"));
	return list.join(", ");
}


QString ardumyActuatorStateToString(const ArduMYActuatorState &s, const ArduMYActuatorValueRepresentation &rep)
{
	QString ret;
	ret+=(
			 //QString(" flags=%1").arg(s.flags,8,2,QChar('0'))
			 ardumyActuatorStateFlagsToString(s)
			 +QStringLiteral("; value=")+ardumyActuatorValueToString(s.value,rep)
		 );
	return ret;
}


QString ardumyActuatorToString(const ArduMYActuator &a)
{
	QString ret;
	ret+=QStringLiteral("config{")+ardumyActuatorConfigToString(a.config)+QStringLiteral("}, state{")+ardumyActuatorStateToString(a.state, a.config.representation)+QStringLiteral("}");
	return ret;
}


QString ardumyActuatorSetToString(const ArduMYActuatorSet &set)
{
	QString ret;
	ret+=(QString("ActuatorSet with size ")+QString::number(set.size())+QStringLiteral(":\n"));
	const auto size=set.size();
	for(size_t i=0; i<size; ++i) {
		const ArduMYActuator &a=set[i];
		ret+=QStringLiteral(" + ") + ardumyActuatorToString(a)+QStringLiteral("\n");
	}
	return ret;
}



QString ardumyActuatorNameToString(const ArduMYActuatorConfig &c)
{
	QString name;
	const qint8 arraySize=sizeof(c.nickName)/sizeof(int8_t);
	const qint8 maxCharacters=arraySize-1;
	int i=0;
	for(i=0; i<maxCharacters; ++i) {
		auto ch=c.nickName[i];
		if('\0'==ch) {
			break;
		}
		name[i]=ch;
	}
	return name;
}

void ardumyActuatorNameFromString(ArduMYActuatorConfig &config ,const QString &name)
{
	const quint32 maxSz=(sizeof(ArduMYActuatorConfig::nickName)/sizeof(int8_t))-1;// NOTE: -1 means we make space for the null-terminator character
	const quint32 nSz=name.size();
	const auto sz=qMin(maxSz, nSz);
	size_t i=0;
	for(; i<sz; ++i) {
		config.nickName[i]=name.at(i).toLatin1();
	}
	// Pad/Postfix the null-terminator
	for(; i<maxSz; ++i) {
		config.nickName[i]='\0';
	}
}
/*
ArduMYActuatorSet &actuators;
ArduMYParserState currentCommand;
const uint8_t magic[4];
uint8_t byteIndex;
ArduMYActuatorConfigSerializer actuatorConfigSerializer;
int16_t actuatorConfigIndex;
ArduMYActuatorValueSerializer actuatorValuesSerializer;
*/

QString ardumyCommandSerializerToString(const ArduMYCommandSerializer &cs)
{
	QString out="CommandSerializer{ actuators="+QString::number(cs.actuators.size())+", currentCommand="+ardumyParserStateToString(cs.currentCommand)+", actuators="+QString::number(cs.actuators.size())+", }";
	return out;
}

QString ardumyCommandParserToString(const ArduMYCommandParser &cp)
{
	QString out="CommandParser{ actuators="+QString::number(cp.actuators.size())+", }";
	return out;

}
