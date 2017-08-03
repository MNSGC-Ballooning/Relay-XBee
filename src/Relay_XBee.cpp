#include "Relay_XBee.h"

//constructor for hardware serial connection
XBee::XBee(HardwareSerial* port, String id) {
	hardPort = port;
	usingSoftSerial = false;
	this->id = id;
}

//constructor for software serial connection
#ifdef SoftwareSerial_h
XBee::XBee(SoftwareSerial* port, String id) {
	softPort = port;
	usingSoftSerial = true;
	this->id = id;
}
#endif

//call during setup to begin appropriate serial connection
void XBee::begin(long baud) {
#ifdef SoftwareSerial_h
	if (usingSoftSerial)
		softPort->begin(baud);
	else
#endif
		hardPort->begin(baud);
}

//sets cooldown between repeat commands
void XBee::setCooldown(byte cooldown) {
	this->cooldown = cooldown;
}

//sends input String to Relay, with id and correct formatting
void XBee::send(String message) {
	unsigned int transmissionLength = id.length() + message.length() + 2;
	char* transmission = new char[transmissionLength];
	int pos = 0;
	for (byte i = 0; i < id.length(); i++) {
		transmission[pos] = id.charAt(i);
		pos++;
	}
	transmission[pos] = ';';
	pos++;
	for (byte i = 0; i < message.length(); i++) {
		transmission[pos] = message.charAt(i);
		pos++;
	}
	transmission[pos] = '!';
	println(transmission, transmissionLength);
	delete[] transmission;
}

//sends input char array to Relay, with id and correct formatting. Use this version for longer transmisssions to save memory
void XBee::send(char* message, int messageLength) {
	unsigned int transmissionLength = id.length() + messageLength + 2;
	char* transmission = new char[transmissionLength];
	int pos = 0;
	for (byte i = 0; i < id.length(); i++) {
		transmission[pos] = id.charAt(i);
		pos++;
	}
	transmission[pos] = ';';
	pos++;
	for (byte i = 0; i < messageLength; i++) {
		transmission[pos] = message[i];
		pos++;
	}
	transmission[pos] = '!';
	println(transmission, transmissionLength);
	delete[] transmission;
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
	if (!complete || command.equals(lastCom) && (millis() - comTime < cooldown * 1000)) return "";
	byte split = command.indexOf('?');	//question mark separates id from command
	if (!(command.substring(0, split)).equals(id)) return "";	//returns empty string if wrong id
	lastCom = command;		//once a valid command is received, save it to avoid repeats...
	comTime = millis();		//...for a short time
	acknowledge();			//inform relay that command was received
	return (command.substring(split + 1, command.length() -1));	//return just the command portion as a string
}

//used to tell the Relay that the transmission was received
void XBee::acknowledge() {
	byte len = id.length() + 1;
	char* ack = new char[len];
	for (byte i = 0; i < len - 1; i++) {
		ack[i] = id.charAt(i);
	}
	ack[len-1] = '\n';
	println(ack, len);
	delete[] ack;
}

//checks appropriate serial connection for available data
bool XBee::isAvailable() {
#ifdef SoftwareSerial_h
	if (usingSoftSerial)
		return softPort->available() > 0;
	else
#endif
		return hardPort->available() > 0;
}

//calls println() function of appropriate serial connection
void XBee::println(char* data, unsigned int dataLength) {
#ifdef SoftwareSerial_h
	if (usingSoftSerial){
		softPort->write(data, dataLength);
		softPort->println();
	}
	else
#endif
		hardPort->write(data, dataLength);
		hardPort->println();
}

//calls read() function of appropriate serial connection
char XBee::read() {
#ifdef SoftwareSerial_h
	if (usingSoftSerial)
		return char(softPort->read());
	else
#endif
		return char(hardPort->read());
}
