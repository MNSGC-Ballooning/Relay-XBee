#ifndef Relay_XBee_h
#define Relay_XBee_h

#include <Arduino.h>
#include <SoftwareSerial.h>



class XBee {
	public:
		XBee(HardwareSerial* port, String id);
		XBee(SoftwareSerial* port, String id);
		void begin(long baud);
		void setCooldown(byte cooldown);
		void send(String message);
		String receive();
	private:
		HardwareSerial* hardPort;
		SoftwareSerial* softPort;
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
