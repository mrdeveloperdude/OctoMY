#include "ArduMY.hpp"


#include "ArduMYCommandParser.hpp"
#include "ArduMYMain.hpp"


ArduMYCommandParser parser;
ArduMYMain hwServos(parser);

const char compile_date[] = __DATE__ " " __TIME__;


void setup()
{
	Serial.begin(115200);
	while (!Serial);
	parser.setup();
	hwServos.setup();
	Serial.print("Compiled@");
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
			Serial.print("COMMAND CHANGED FROM ");
			Serial.print(oldCom);
			Serial.print(" TO ");
			Serial.println(nuCom);
		}
	}
	hwServos.update(unow);
}

