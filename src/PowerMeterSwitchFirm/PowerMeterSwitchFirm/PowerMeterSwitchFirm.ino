/*
 Name:		PowerMeterSwitchFirm.ino
 Created:	11/23/2016 12:54:25 AM
 Author:	Victor
*/

#include "PowerMeter_CS5460Lib.h"
#include <RingBuf.h>

// the setup function runs once when you press reset or power the board
void setup() {
	PowerMeter_CS5460::initialize(4, 5);
}

// the loop function runs over and over again until power down or reset
void loop() {
	PowerMeter_CS5460::update();
}
