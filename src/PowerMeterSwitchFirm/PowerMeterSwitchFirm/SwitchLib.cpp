/*
Name:		SwitchLib.cpp
Created:	11/22/2016 1:49:37 AM
Author:	Victor
Editor:	http://www.visualmicro.com
*/

#include "SwitchLib.h"

SwitchLib::SwitchLib(int switchPin)
{
	_switchPin = switchPin;
}

void SwitchLib::update()
{
	if (onTime == 0 || 
		!_isOn)
	{
		return;
	}

	auto elapsed = millis() - _countInit;
	if (elapsed > onTime)
	{
		off();
	}
}

bool inline SwitchLib::isOn()
{
	return _isOn;
}

void SwitchLib::on()
{
	_countInit = millis();
	digitalWrite(_switchPin, HIGH);
	_isOn = true;
}

void SwitchLib::off()
{
	digitalWrite(_switchPin, LOW);
	_isOn = false;
}

void SwitchLib::toggle()
{
	_isOn ? off() : on();
}

