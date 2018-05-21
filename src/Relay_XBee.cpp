#include "Relay_XBee.h"

//Constructor for hardware serial connection. Does not alter XBee settings
XBee::XBee(HardwareSerial* port, String id):
  hardPort(port), id(id) {
  usingSoftSerial = false;
}

//Constructor for hardware serial connection. Configures XBee to communicate with given stack.
XBee::XBee(HardwareSerial* port, String id, char stack):
  hardPort(port), id(id) {
    usingSoftSerial = false;
    switch (stack) {
      case 'A':
        PANid = "AAAA";
        break;
      case 'B':
        PANid = "BBBB";
        break;
      case 'C':
        PANid = 'CCCC';
        break;
    }
}

#ifdef SoftwareSerial_h
//Constructor for software serial connection. Does not alter XBee settings
XBee::XBee(SoftwareSerial* port, String id):
  softPort(port), id(id) {
  usingSoftSerial = true;
}

//Constructor for software serial connection. Configures XBee to communicate with given stack.
XBee::XBee(SoftwareSerial* port, String id, char stack):
  softPort(port), id(id) {
    usingSoftSerial = true;
    switch (stack) {
      case 'A':
        PANid = "AAAA";
        break;
      case 'B':
        PANid = "BBBB";
        break;
      case 'C':
        PANid = 'CCCC';
        break;
    }
}
#endif

//call during setup to begin appropriate serial connection
void XBee::initialize() {
#ifdef SoftwareSerial_h
  if (usingSoftSerial)
  {
    softPort->begin(9600);
    softPort->setTimeout(10);
  }
  else
#endif
  {
    hardPort->begin(9600);
    hardPort->setTimeout(10);
  }
  if (PANid.equals("")) return;
  String response;
    for (byte i = 0; !enterATmode() && (i < 10); i++) {}
    for (byte i = 0; !response.equals("OK") && (i < 10); i++) {
      response = atCommand("ATID"+PANid);
    }
    response = "";
    for (byte i = 0; !response.equals("OK") && (i < 10); i++) {
      response = atCommand("ATDL0");
    }
    response = "";
    for (byte i = 0; !response.equals("OK") && (i < 10); i++) {
      response = atCommand("ATMY1");
    }
    response = "";
    for (byte i = 0; !response.equals("OK") && (i < 10); i++) {
      response = atCommand("ATWR");
    }
    response = "";
    for (byte i = 0; !response.equals("OK") && (i < 10); i++) {
      response = atCommand("ATCN");
    }
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
  id.toCharArray(transmission, id.length() + 1);
  pos += id.length();
  transmission[pos] = ';';
  pos++;
  message.toCharArray(transmission + pos, message.length() + 1);
  pos += message.length();
  transmission[pos] = '!';
  println(transmission, transmissionLength);
  delete[] transmission;
}

//sends input char array to Relay, with id and correct formatting. Use this version for longer transmisssions to save memory
void XBee::send(char* message, int messageLength) {
  unsigned int transmissionLength = id.length() + messageLength + 2;
  char* transmission = new char[transmissionLength];
  int pos = 0;
  id.toCharArray(transmission, id.length() + 1);
  pos += id.length();
  transmission[pos] = ';';
  pos++;
  for (byte i = 0; i < messageLength; i++, pos++) {
    transmission[pos] = message[i];
  }
  transmission[pos] = '!';
  println(transmission, transmissionLength);
  delete[] transmission;
}

//sends gps data by xBee in a format the ground station can parse
void XBee::sendGPS(byte hour, byte minute, byte second, float lat, float lon, float alt, byte sats) {
  char* transmission = new char[42 + id.length() + 2];
  int pos = 0;
  id.toCharArray(transmission, id.length() + 1);
  pos += id.length();
  transmission[pos] = ';';
  pos++;
  //Add each parameter to the char array in order
  //hour
  String str = String(hour);
  for (byte i = str.length(); i < 2; i++, pos++) {
    transmission[pos] = '0';
  }
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = ':';
  pos++;
  
  //minute
  str = String(minute);
  for (byte i = str.length(); i < 2; i++, pos++) {
    transmission[pos] = '0';
  }
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = ':';
  pos++;
  
  //second
  str = String(second);
  for (byte i = str.length(); i < 2; i++, pos++) {
    transmission[pos] = '0';
  }
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = ',';
  pos++;
  
  //lat
  str = String(lat, 4);
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = ',';
  pos++;
  
  //lon
  str = String(lon, 4);
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = ',';
  pos++;
  
  //alt
  str = String(alt, 1);
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = ',';
  pos++;
  
  //sats
  str = String(sats);
  str.toCharArray(transmission + pos, str.length() + 1);
  pos += str.length();
  
  transmission[pos] = '!';
  pos++;
  println(transmission, pos);
  delete[] transmission;
}

//parses incoming xBee transmissions for valid commands with xBee's id
//and returns just the command as a string if found
String XBee::receive() {
  String command = read();
  //return an empty string if command is a repeat, since the Relay transmits a command repeatedly until acknowledged
  if (command.equals(lastCom) && (millis() - comTime < cooldown * 1000)) return "";
  byte split = command.indexOf('?');	//question mark separates id from command
  if (!(command.substring(0, split)).equals(id)) return "";	//returns empty string if wrong id
  lastCom = command;		//once a valid command is received, save it to avoid repeats...
  comTime = millis();		//...for a short time
  acknowledge();			//inform relay that command was received
  return (command.substring(split + 1, command.length()));	//return just the command portion as a string
}

//Call this function before attempting to send AT commands.
//Returns true if successful
bool XBee::enterATmode() {
  String response;
  #ifdef SoftwareSerial_h
  if (usingSoftSerial) {
    softPort->print("+++");
    delay(3000);
    response = softPort->readStringUntil('\r');
  }
  else
#endif
  {
    hardPort->print("+++");
    delay(3000);
    response = hardPort->readStringUntil('\r');
  }
  return response.equals("OK");
}

String XBee::atCommand(String command) {
#ifdef SoftwareSerial_h
  if (usingSoftSerial) {
    softPort->print(command + '\r');
    return softPort->readStringUntil('\r');
  }
  else
#endif
  {
    hardPort->print(command);
    return hardPort->readStringUntil('\r');
  }
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
  {
    hardPort->write(data, dataLength);
    hardPort->println();
  }
}

//calls read() function of appropriate serial connection
String XBee::read() {
#ifdef SoftwareSerial_h
  if (usingSoftSerial)
    return softPort->readStringUntil('\n');
  else
#endif
    return hardPort->readStringUntil('\n');
}