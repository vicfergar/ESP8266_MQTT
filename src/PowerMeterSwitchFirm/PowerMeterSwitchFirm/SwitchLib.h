/*
Name:		SwitchLib.h
Created:	11/22/2016 1:49:37 AM
Author:	Victor
Editor:	http://www.visualmicro.com
*/

#ifndef _SWITCHLIB_h
#define _SWITCHLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class SwitchLib
{
 public:
	 uint32_t onTime;

	 SwitchLib(int switchPin);
	 void update();
	 bool isOn();
	 void on();
	 void off();
	 void toggle();

private:
	int _switchPin;
	bool _isOn;
	unsigned long _countInit;
};

#endif

