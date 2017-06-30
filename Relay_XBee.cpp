#include "Relay_XBee.h"

XBee::XBee(HardwareSerial* port, String id) {
	hardPort = port;
	usingSoftSerial = false;
	this->id = id;
}

XBee::XBee(SoftwareSerial* port, String id) {
	softPort = port;
	usingSoftSerial = true;
	this->id = id;
}

XBee::begin(long baud) {
	if (usingSoftSerial) softPort->begin(baud);
	else hardPort->begin(baud);
}

XBee::setCooldown(byte cooldown) {
	this->cooldown = cooldown;
}

void XBee::send(String message) {
	println(id + ";" + message + "!");
}

String XBee::receive() {
	bool complete = false;
	String command = "";
	char inChar;
	while (isAvailable()) {
		inChar = read();
		if (inChar != ' ') command += inChar;
		if (inChar == '!') {
			complete = true;
			break;
		}
	}
	if (!complete || command.equals(lastCommand) && (millis() - comTime < cooldown * 1000)) return "";
	byte split = command.indexOf('?');
	if (!(command.substring(0, split)).equals(id)) return "";
	lastCommand = command;
	commandTime = millis();
	acknowledge();
	return (command.substring(split + 1, command.length() -1));
}

void XBee::acknowledge() {
	println(id + "\n");
}

bool XBee::isAvailable)() {
	if (usingSoftSerial) return softPort->available() > 0;
	else return hardPort->available() > 0;
}

void XBee::println(String data) {
	if (usingSoftSerial) softPort->println(data);
	else hardPort->println(data);
}

char XBee::read() {
	if (usingSoftSerial) return char(softPort->read());
	else return char(hardPort->read());
}