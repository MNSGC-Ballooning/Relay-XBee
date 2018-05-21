/* Sample code to read an analog temperature sensor and beacon the reading to the relay once per second
 */

#include <Relay_XBee.h>

//define data pin for temp sensor
#define tempPin A0

//XBee object needs a serial line to read
SoftwareSerial ss = SoftwareSerial(2,3);

//Pass a reference to this serial line and chosen ID to the XBee constructor
//Best practice is to keep the ID short: 2-4 characters, capital A-Z and 0-9 only
XBee xBee = XBee(&ss, "ME");
//Alternately, use a hard serial line if the microcontroller supports it
//XBee xBee = XBee(&Serial1, "ME");

//global timer variable for loop
unsigned long timer = 0;

void setup() {
  //Begin XBee communication at specified baud rate. Should be 9600 unless previously set otherwise.
  xBee.initialize();
}

void loop() {
  //execute loop only once per second
  if (millis() - timer > 1000) {
    timer = millis();
	//read temp sensor
    float temp = (analogRead(tempPin)*(5.0/1024)-.5)/.01;
	//send data to ground
    xBee.send("Temp: " + String(temp));
	
	//For large data transmissions, a char array can be used instead to save memory:
	/*
	String prefix = "Temp: ";
	String tempStr = String(temp);
	byte len = 5 + tempStr.length();
	char* data = new char[len];
	byte pos = 0;
	for (byte i = 0; i < prefix.length(); i++) {
      data[pos] = prefix.charAt(i);
	  pos++;
	}
	for (byte i = 0; i < tempStr.length(); i++) {
	  data[pos] = tempStr.charAt(i);
	  pos++;
	}
	xBee.send(data, len);
	delete[] data;
	*/
  }
}