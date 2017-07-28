# Relay_XBee

A library for XBees communicating with the MNSGC Still Image and Relay system.
Encapsulates all protocol details into simple send and receive functions.

## Serial Connection Notes

This library is designed to support both hard and software serial connections to gps modules.
By default, this library automatically #includes the SoftwareSerial.h library.
If building for a board that doesn't support SoftwareSerial, simply //comment out the
#include statement on line 7 of [Relay_XBee.h](src/Relay_XBee.h).
This can also be done to save memory if no SoftwareSerial connections are used.