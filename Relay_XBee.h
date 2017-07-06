#ifndef Relay_XBee_h
#def Relay_XBee_h

#include <Arduino.h>
#include <SoftwareSerial.h>



class XBee {
	public:
		XBee(HardwareSerial* port, String id);
		XBee(SoftwareSerial* port, String id);
		begin(long baud);
		setCooldown(byte cooldown);
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
		String read();
}

#endif