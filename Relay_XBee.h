#ifndef Relay_XBee_h
#define Relay_XBee_h

#include <Arduino.h>

#include <SoftwareSerial.h> //Comment this out if inclusion causes errors, or using hard serial and want to save space

class XBee {
	public:
		XBee(HardwareSerial* port, String id);
#ifdef SoftwareSerial_h
		XBee(SoftwareSerial* port, String id);
#endif
		void begin(long baud);
		void setCooldown(byte cooldown);
		void send(String message);
		String receive();
	private:
		HardwareSerial* hardPort;
#ifdef SoftwareSerial_h
		SoftwareSerial* softPort;
#endif
		bool usingSoftSerial;
		String id;
		String lastCom;
		byte cooldown = 10; //default time before accepting repeat transmissions is 10s
		unsigned long comTime;
		void acknowledge();
		bool isAvailable();
		void println(String data);
		char read();
};

#endif
