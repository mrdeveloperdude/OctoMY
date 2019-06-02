#include "ArduMYTypeConversions.hpp"

#include "ardumy_arduino/ArduMYActuatorState.hpp"
#include "ardumy_arduino/ArduMYActuatorConfig.hpp"
#include "ardumy_arduino/ArduMYActuator.hpp"

#include "ardumy_arduino/ArduMYActuatorSet.hpp"
#include "ardumy_arduino/ArduMYCommandSerializer.hpp"
#include "ardumy_arduino/ArduMYCommandParser.hpp"


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
	case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
		return val.bit?"true":"false";
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
		return QString::number((quint8)val.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
		return QString::number((quint16)val.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
		return QString::number((quint32)val.doubleWord);
	}
	break;
	//Default to strictest possible equality when representation is unknown
	default:
	case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT):
	case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
		return QString::number((quint64)val.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
		return QString::number((float)val.singlePrecision);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
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

const ArduMYActuatorValueRepresentation DEFAULT_REPRESENTATION=VALREP_REPRESENTATION_COUNT;



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
	case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
		val.bit=variant.toBool();
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
		val.byte=variant.toUInt();
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
		val.word=variant.toUInt();
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
		val.doubleWord=variant.toUInt();
	}
	break;
	//Default to strictest possible equality when representation is unknown
	default:
	case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT):
	case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
		val.quadWord=variant.toULongLong();
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
		val.singlePrecision=variant.toFloat();
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
		val.doublePrecision=variant.toDouble();
	}
	break;
	}
}


QVariant ardumyActuatorValueToVariant(const ArduMYActuatorValue &val, const ArduMYActuatorValueRepresentation &representation)
{
	switch(representation) {
	case(ArduMYActuatorValueRepresentation::VALREP_BIT): {
		return QVariant((bool)val.bit);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_BYTE): {
		return QVariant((quint8)val.byte);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_WORD): {
		return QVariant((quint16)val.word);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_WORD): {
		return QVariant((quint32)val.doubleWord);
	}
	break;
	//Default to strictest possible equality when representation is unknown
	default:
	case(ArduMYActuatorValueRepresentation::VALREP_REPRESENTATION_COUNT):
	case(ArduMYActuatorValueRepresentation::VALREP_QUAD_WORD): {
		return QVariant((quint64)val.quadWord);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_SINGLE_FLOAT): {
		return QVariant((float)val.singlePrecision);
	}
	break;
	case(ArduMYActuatorValueRepresentation::VALREP_DOUBLE_FLOAT): {
		return QVariant((double)val.doublePrecision);
	}
	break;
	}
	return QVariant();
}


ArduMYActuatorValueRepresentation ardumyActuatorValueRepresentationFromString(const QString &s)
{
#define  ardumyActuatorTypeFromStringCASE(A) if (#A == s)return A
	ardumyActuatorTypeFromStringCASE(VALREP_BIT);
	ardumyActuatorTypeFromStringCASE(VALREP_BYTE);
	ardumyActuatorTypeFromStringCASE(VALREP_WORD);
	ardumyActuatorTypeFromStringCASE(VALREP_DOUBLE_WORD);
	ardumyActuatorTypeFromStringCASE(VALREP_QUAD_WORD);
	ardumyActuatorTypeFromStringCASE(VALREP_SINGLE_FLOAT);
	ardumyActuatorTypeFromStringCASE(VALREP_DOUBLE_FLOAT);
	ardumyActuatorTypeFromStringCASE(VALREP_REPRESENTATION_COUNT);

#undef ardumyActuatorTypeFromStringCASE
	return VALREP_REPRESENTATION_COUNT;
}


QString ardumyActuatorValueRepresentationToString(const ArduMYActuatorValueRepresentation &rep)
{
#define  ArduMYActuatorTypeToStringCASE(A) case (A):return #A
	switch(rep) {
	default:
		return "UNKNOWN";
		ArduMYActuatorTypeToStringCASE(VALREP_BIT);
		ArduMYActuatorTypeToStringCASE(VALREP_BYTE);
		ArduMYActuatorTypeToStringCASE(VALREP_WORD);
		ArduMYActuatorTypeToStringCASE(VALREP_DOUBLE_WORD);
		ArduMYActuatorTypeToStringCASE(VALREP_QUAD_WORD);
		ArduMYActuatorTypeToStringCASE(VALREP_SINGLE_FLOAT);
		ArduMYActuatorTypeToStringCASE(VALREP_DOUBLE_FLOAT);
		ArduMYActuatorTypeToStringCASE(VALREP_REPRESENTATION_COUNT);

	}
#undef ArduMYActuatorTypeToStringCASE
}


QString ardumyActuatorValueToString(const ArduMYActuatorValue &v, const ArduMYActuatorValueRepresentation &rep)
{
	QString ret=ardumyActuatorValueRepresentationToString(rep)+" ( ";
	switch(rep) {
	case(VALREP_BIT):
		ret+=(v.bit?"TRUE":"FALSE");
		break;
	case(VALREP_BYTE):
		ret+=QString::number(v.byte);
		break;
	case(VALREP_WORD):
		ret+=QString::number(v.word);
		break;
	case(VALREP_DOUBLE_WORD):
		ret+=QString::number(v.doubleWord);
		break;
	case(VALREP_REPRESENTATION_COUNT):
		ret+="REPRESENTATION_COUNT";
		break;
	default:
		ret+="OUT_OF_RANGE!";
		break;
	case(VALREP_QUAD_WORD): {
		ret+=QString::number(v.quadWord);
	}
	break;
	case(VALREP_SINGLE_FLOAT):
		ret+=QString::number(v.singlePrecision);
		break;
	case(VALREP_DOUBLE_FLOAT):
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
			 +QStringLiteral("; nick=")
			 +ardumyActuatorNameToString(c)
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


void diffArdumyActuatorState(const ArduMYActuatorState &a, const ArduMYActuatorState &b, const ArduMYActuatorValueRepresentation &rep)
{
	// Mask to remove ephemeral parts of flags (currently only isDirty)
	const uint16_t temp=(1<<1);
	const uint16_t mask=(~temp);


	const uint16_t maskedFlags=(a.flags & mask);
	const uint16_t otherMaskedFlags=(b.flags & mask);

	if( maskedFlags != otherMaskedFlags) {
		qDebug()<<"A state flags "<<ardumyActuatorStateFlagsToString(a) <<" are different than B state flags "<<ardumyActuatorStateFlagsToString(b);
	}
	if(!a.value.isEqual(b.value,rep)) {
		qDebug()<<"A value "<<ardumyActuatorValueToString(a.value,rep) <<" are different than B value "<<ardumyActuatorValueToString(b.value,rep);
	}
}


void diffArdumyActuatorConfig(const ArduMYActuatorConfig &a, const ArduMYActuatorConfig &b)
{

	// Mask to remove ephemeral parts of flags (currently only isDirty)
	const uint16_t temp=(1<<9);
	const uint16_t mask=(~temp);

	const uint16_t maskedFlags=(a.flags & mask);
	const uint16_t otherMaskedFlags=(b.flags & mask);

	if( maskedFlags != otherMaskedFlags) {
		qDebug()<<"A flags "<<ardumyActuatorConfigFlagsToString(a) <<" are different than B flags "<<ardumyActuatorConfigFlagsToString(b);
	}
	if(a.type!=b.type) {
		qDebug()<<"A type "<<ardumyActuatorTypeToString(a.type) <<" is different than B type "<<ardumyActuatorTypeToString(b.type);
	}
	if(a.representation !=b.representation) {
		qDebug()<<"A type "<<ardumyActuatorValueRepresentationToString(a.representation) <<" is different than B representation "<<ardumyActuatorValueRepresentationToString(b.representation);
	}
	// TODO: factorize to find "different but same" like 2:2, 3:3 etc.
	if(a.hasGearRatio()&& b.hasGearRatio() && (a.gearRatioNumerator != b.gearRatioNumerator) ) {
		qDebug()<<"A gear ratio numerator "<<a.gearRatioNumerator <<" is different than B gear ratio numerator "<<b.gearRatioNumerator;
	}
	if(a.hasGearRatio()&& b.hasGearRatio() && (a.gearRatioDenominator!= b.gearRatioDenominator) ) {
		qDebug()<<"A gear ratio denominator "<<a.gearRatioDenominator <<" is different than B gear ratio denominator "<<b.gearRatioDenominator;
	}
	if(a.hasPositionFeedback() && b.hasPositionFeedback() && (a.positionFeedbackPin != b.positionFeedbackPin)) {
		qDebug()<<"A positionFeedbackPin "<<a.positionFeedbackPin <<" is different than B positionFeedbackPin "<<b.positionFeedbackPin;
	}
	if(a.hasTachometer() && b.hasTachometer() && (a.tachometerPin != b.tachometerPin)) {
		qDebug()<<"A tachometerPin "<<a.tachometerPin <<" is different than B tachometerPin "<<b.tachometerPin;
	}
	if(a.hasIncrementalEncoder() && b.hasIncrementalEncoder() && (a.incrementalEncoderPinA != b.incrementalEncoderPinA)) {
		qDebug()<<"A incrementalEncoderPinA "<<a.incrementalEncoderPinA <<" is different than B incrementalEncoderPinA "<<b.incrementalEncoderPinA;
	}
	if(a.hasIncrementalEncoder() && b.hasIncrementalEncoder() && (a.incrementalEncoderPinB != b.incrementalEncoderPinB)) {
		qDebug()<<"A incrementalEncoderPinB "<<a.incrementalEncoderPinB <<" is different than B incrementalEncoderPinB "<<b.incrementalEncoderPinB;
	}
	if(a.hasIncrementalEncoder() && b.hasIncrementalEncoder() && (a.incrementalencoderDebounceCount != b.incrementalencoderDebounceCount)) {
		qDebug()<<"A incrementalencoderDebounceCount "<<a.incrementalencoderDebounceCount <<" is different than B incrementalencoderDebounceCount "<<b.incrementalencoderDebounceCount;
	}
	if(a.hasLimitSwitchStart() && b.hasLimitSwitchStart() && (a.limitSwitchPinStart != b.limitSwitchPinStart)) {
		qDebug()<<"A limitSwitchPinStart "<<a.limitSwitchPinStart <<" is different than B limitSwitchPinStart "<<b.limitSwitchPinStart;
	}
	if(a.hasLimitSwitchEnd() && b.hasLimitSwitchEnd() && (a.limitSwitchPinEnd != b.limitSwitchPinEnd)) {
		qDebug()<<"A limitSwitchPinEnd "<<a.limitSwitchPinEnd <<" is different than B limitSwitchPinEnd "<<b.limitSwitchPinEnd;
	}
	if((a.hasLimitSwitchStart() || a.hasLimitSwitchEnd()) && (b.hasLimitSwitchStart() || b.hasLimitSwitchEnd())  && (a.limitSwitchDebounceCount != b.limitSwitchDebounceCount)) {
		qDebug()<<"A limitSwitchDebounceCount "<<a.limitSwitchDebounceCount <<" is different than B limitSwitchDebounceCount "<<b.limitSwitchDebounceCount;
	}

	if(a.type == b.type) {
		switch(a.type) {

		case(STEP_MOTOR): {
			if(a.stepMotorPhaseCount!= b.stepMotorPhaseCount) {
				qDebug()<<"A stepMotorPhaseCount "<<a.stepMotorPhaseCount <<" is different than B stepMotorPhaseCount "<<b.stepMotorPhaseCount;
			}
			if(a.stepMotorStepsPerRotation!= b.stepMotorStepsPerRotation) {
				qDebug()<<"A stepMotorStepsPerRotation "<<a.stepMotorStepsPerRotation <<" is different than B stepMotorStepsPerRotation "<<b.stepMotorStepsPerRotation;
			}
		}
		break;

		case(RC_SERVO): {
			if(a.rcServoPin!= b.rcServoPin) {
				qDebug()<<"A rcServoPin "<<a.rcServoPin <<" is different than B rcServoPin "<<b.rcServoPin;
			}
		}
		break;
		case(DC_MOTOR):
		case(RELAY):
		case(TYPE_COUNT):
		default:
			break;

		}
	}

	if(a.representation ==b.representation) {
		if(!a.rangeStart.isEqual(b.rangeStart,a.representation)) {
			qDebug()<<"A rangeStart "<<	ardumyActuatorValueToString(a.rangeStart, a.representation)<<" is different than B rangeStart "<<ardumyActuatorValueToString(b.rangeStart, a.representation);
		}
		if(!a.rangeSpan.isEqual(b.rangeSpan,a.representation)) {
			qDebug()<<"A rangeSpan "<<	ardumyActuatorValueToString(a.rangeSpan, a.representation)<<" is different than B rangeSpan "<<ardumyActuatorValueToString(b.rangeSpan, a.representation);
		}
	}
}


void diffArdumyActuator(const ArduMYActuator &a, const ArduMYActuator &b)
{
	if(!a.config.isEqual(b.config)) {
		diffArdumyActuatorConfig(a.config, b.config);
	}
	if(a.config.representation == b.config.representation && (!a.state.isEqual(b.state, a.config.representation) )) {
		diffArdumyActuatorState(a.state, b.state, a.config.representation);
	}
}


void diffArdumyActuatorSet(const ArduMYActuatorSet &setA, const ArduMYActuatorSet &setB)
{
	if(setA.size()!=setB.size()) {
		qWarning()<<"SET SIZE DIFFERS: "<<setA.size()<<" vs. "<<setB.size();
	}

	const uint32_t sz=qMin(setA.size(), setB.size());
	for(uint32_t i=0; i<sz; ++i) {
		const ArduMYActuator &a=setA[i];
		const ArduMYActuator &b=setB[i];
		if(!a.isEqual(b)) {
			qWarning()<<"ACTUATOR @ INDEX "<<i<< " DIFFERS: ";
			diffArdumyActuator(a,b);
		}
	}
}
