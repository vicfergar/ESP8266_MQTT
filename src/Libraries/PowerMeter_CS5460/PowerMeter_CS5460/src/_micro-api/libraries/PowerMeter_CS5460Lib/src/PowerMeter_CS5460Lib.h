/*
 Name:		PowerMeter_CS5460Lib.h
 Created:	11/22/2016 7:52:37 PM
 Author:	Victor
 Editor:	http://www.visualmicro.com
*/

#ifndef _PowerMeter_CS5460Lib_h
#define _PowerMeter_CS5460Lib_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <RingBuf.h>

#define REGISTERS_PER_FRAME 3
#define TIMER_RESET_US 20000L				// Goal to achieve about 10-30ms timer interval
#define BUFSIZE (REGISTERS_PER_FRAME * 10)  // Grab last 10 frames

#define VOLTAGE_RANGE 0.250					// Full scale V channel voltage
#define CURRENT_RANGE 0.050					// Full scale I channel voltage (PGA 50x instead of 10x)
#define VOLTAGE_DIVIDER 450220.0/220.0		// Input voltage channel divider (R1+R2/R2)
#define CURRENT_SHUNT 620					// Empirically obtained multiplier to scale Vshunt drop to I    
#define FLOAT24 16777216.0					// 2^24 (converts to float24)
#define POWER_MULTIPLIER 1 / 512.0			// Energy->Power divider; not sure why, but seems correct. Datasheet not clear about this.

#define VOLTAGE_MULTIPLIER   (float)  (1 / FLOAT24 * VOLTAGE_RANGE * VOLTAGE_DIVIDER)
#define CURRENT_MULTIPLIER   (float)  (1 / FLOAT24 * CURRENT_RANGE * CURRENT_SHUNT)

class PowerMeter_CS5460
{
public:
	static float voltage;
	static float current;
	static float truePower;
	static float powerFactor;
	
	static volatile int lostDataCount;

	static void initialize(int clkPin, int sdoPin);
	static bool update();

private:
	static int _clkPin;
	static int _sdoPin;

	static volatile uint32_t _sniffRegister;
	static volatile uint8_t _bitsForNextData;
	static volatile bool _syncPulse;
	static volatile bool _readyReceived;
	static RingBuf *_dataBuf;
	
	static void inline resetTimer();
	static void inline clockISR();
	static void inline timerOVF();
};

#endif

