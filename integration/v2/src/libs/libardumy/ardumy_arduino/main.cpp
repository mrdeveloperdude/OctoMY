
// Only build when we are targeting actual Arduino hardware
#ifdef OCTOMY_ARDUINO_TARGET

#include "ardumy_arduino/ArduMY.hpp"


#include "ardumy_arduino/ArduMYCommandParser.hpp"
#include "ardumy_arduino/ArduMYMain.hpp"


ArduMYCommandParser parser;
ArduMYMain hwServos(parser);

const char compile_date[] = __DATE__  __TIME__;


void setup()
{
	Serial.begin(115200);
	while (!Serial);
	parser.setup();
	hwServos.setup();
	Serial.println(compile_date);
	Serial.println("READY");
}


void loop()
{
	uint32_t unow = micros();
	// TODO: Look at throtling serial input as a way to avoid stuttering if that poses a problem
	while (Serial.available()) {
		const char in = (char)Serial.read();
		auto oldCom=parser.currentCommand;
		parser.parse(in);
		auto nuCom=parser.currentCommand;
		if(oldCom!=nuCom) {
			Serial.print("CMD ");
			Serial.print(oldCom);
			Serial.print(" -> ");
			if(OCTOMY_SYNC==nuCom) {
				Serial.println("OUT OF SYNC!");
			} else {
				Serial.println(nuCom);
			}
		}
	}
	hwServos.update(unow);
}

#endif // OCTOMY_ARDUINO_TARGET
