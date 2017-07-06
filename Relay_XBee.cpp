#include "Relay_XBee.h"

//constructor for hardware serial connection
XBee::XBee(HardwareSerial* port, String id) {
	hardPort = port;
	usingSoftSerial = false;
	this->id = id;
}

//constructor for software serial connection
XBee::XBee(SoftwareSerial* port, String id) {
	softPort = port;
	usingSoftSerial = true;
	this->id = id;
}

//call during setup to begin appropriate serial connection
XBee::begin(long baud) {
	if (usingSoftSerial) softPort->begin(baud);
	else hardPort->begin(baud);
}

//sets cooldown between repeat commands
XBee::setCooldown(byte cooldown) {
	this->cooldown = cooldown;
}

//sends input string to Relay, with id and correct formatting
void XBee::send(String message) {
	println(id + ";" + message + "!");
}

//parses incoming xBee transmissions for valid commands with xBee's id
//and returns just the command as a string if found
String XBee::receive() {
	bool complete = false;
	String command = "";
	char inChar;
	while (isAvailable()) {
		inChar = read();
		if (inChar != ' ') command += inChar;	//parse out extra spaces
		if (inChar == '!') {					//exclamation point is terminating character
			complete = true;
			break;
		}
	}
	//return an empty string if command is a repeat, since the Relay transmits a command repeatedly until acknowledged
	if (!complete || command.equals(lastCommand) && (millis() - comTime < cooldown * 1000)) return "";
	byte split = command.indexOf('?');	//question mark separates id from command
	if (!(command.substring(0, split)).equals(id)) return "";	//returns empty string if wrong id
	lastCommand = command;		//once a valid command is received, save it to avoid repeats...
	commandTime = millis();		//...for a short time
	acknowledge();				//inform relay that command was received
	return (command.substring(split + 1, command.length() -1));	//return just the command portion as a string
}

//used to tell the Relay that the transmission was received
void XBee::acknowledge() {
	println(id + "\n");
}

//checks appropriate serial connection for available data
bool XBee::isAvailable)() {
	if (usingSoftSerial) return softPort->available() > 0;
	else return hardPort->available() > 0;
}

//calls println() function of appropriate serial connection
void XBee::println(String data) {
	if (usingSoftSerial) softPort->println(data);
	else hardPort->println(data);
}

//calls read() function of appropriate serial connection
char XBee::read() {
	if (usingSoftSerial) return char(softPort->read());
	else return char(hardPort->read());
}