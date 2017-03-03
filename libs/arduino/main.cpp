#include "ArduMY.hpp"


#include "CommandParser.hpp"
#include "ArduMYMain.hpp"


CommandParser parser;
ArduMYMain hwServos(parser);



void setup() {
  Serial.begin(115200);
  while (!Serial);
  parser.setup();
  hwServos.setup();
  Serial.println("READY");
}


void loop() {
  uint32_t unow = micros();
  // TODO: Look at throtling serial input as a way to avoid stuttering if that poses a problem
  while (Serial.available()) {
	  const char in = (char)Serial.read();
	  parser.parse(in);
  }
  hwServos.update(unow);
}



