#ifndef Relay_XBee_h
#define Relay_XBee_h

#include <Arduino.h>

//Comment this out if inclusion causes errors, or using hard serial and want to save memory
#include <SoftwareSerial.h>

class XBee {
  public:
    XBee(HardwareSerial* port, String id);
    XBee(HardwareSerial* port, String id, char stack);
#ifdef SoftwareSerial_h
    XBee(SoftwareSerial* port, String id);
    XBee(SoftwareSerial* port, String id, char stack);
#endif
    void initialize();
    void setCooldown(byte cooldown);
    void send(String message);
    void send(char* message, int messageLength);
    void sendGPS(byte hour, byte minute, byte second, float lat, float lon, float alt, byte sats);
    String receive();
    bool enterATmode();
    bool exitATmode();
    String atCommand(String command);
  private:
    HardwareSerial* hardPort;
#ifdef SoftwareSerial_h
    SoftwareSerial* softPort;
#endif
    bool usingSoftSerial;
    String id, lastCom;
    String PANid = "";
    byte cooldown = 10; //default time before accepting repeat transmissions is 10s
    unsigned long comTime;
    void acknowledge();
    bool isAvailable();
    void println(char* data, unsigned int dataLength);
    String read();
};

#endif
