/* Sample code to listen for XBee transmissions and respond accordingly
 */

#include <Relay_XBee.h>

//Give the XBee a unique ID. Best practice is to keep it short: 2-4 characters, capital A-Z and 0-9 only
//This can be done directly in the constructor, but making it global allows it to be used elsewhere if needed
const String ID = "ME";

//XBee object needs a serial line to read
SoftwareSerial ss = SoftwareSerial(2,3);

//Pass a reference to this serial line and chosen ID to the XBee constructor
XBee xBee = XBee(&ss, ID);
//Alternately, use a hard serial line if the microcontroller supports it
//XBee xBee = XBee(&Serial1, ID);

void setup() {
  //Optional: set a specific cooldown time (in seconds) during which the XBee will ignore repeats of the same command
  //Default is 10 seconds if function isn't called
  //xBee.setCooldown(20);

  //Begin XBee communication at specified baud rate. Should be 9600 unless previously set otherwise.
  xBee.initialize();
}

void loop() {
  //Save string received from xBee
  String command = xBee.receive();
  //if string is empty, then no command with the xBee's ID was sent
  if (command.equals("")) return;

  //all possible commands go in a series of if-else statements, where the correct action is taken in each case
  if (command.equals("HI"))
    xBee.send("Hello");
  else if (command.equals("NAME"))
    xBee.send(ID);
  else  //if a command other than those expected was sent, inform the sender of the problem
    xBee.send("Error: " + command + " - Command not recognized");
}