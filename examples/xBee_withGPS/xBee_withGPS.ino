/* Sample code to read a gps module and beacon the data to the ground station
 */

#include <Relay_XBee.h>
#include <TinyGPS.h>

//variables that gps data will be written to
float lat, lon, alt;
int sats, year;
byte month, day, hour, minute, second, hundreth;
unsigned long fixAge;

//Serial connections for GPS and XBee
SoftwareSerial gpsSerial = SoftwareSerial(2,3);
SoftwareSerial xBeeSerial = SoftwareSerial(4,5);

//GPS and XBee objects
TinyGPS gps;
XBee xBee = XBee(&xBeeSerial, "GPS");

void setup() {
  //begin both serial connections
  gpsSerial.begin(9600);
  xBee.begin(9600);
}

void loop() {
  //read gps data, check for new NMEA strings
  bool newData = false;
  while(gpsSerial.available() > 0) {
    if(gps.encode(gpsSerial.read()))
      newData = true;
  }
  //if new string is received, update gps variables...
  if (newData) {
    gps.f_get_position(&lat, &lon, &fixAge);
    alt = gps.f_altitude();
    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundreth, &fixAge);
    sats = gps.satellites();
    //...and send them by xBee
    xBee.sendGPS(hour, minute, second, lat, lon, alt, sats);
  }
}