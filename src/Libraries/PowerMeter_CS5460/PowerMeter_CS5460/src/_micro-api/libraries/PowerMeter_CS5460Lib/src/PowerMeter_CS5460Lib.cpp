/*
 Name:		PowerMeter_CS5460Lib.cpp
 Created:	11/22/2016 7:52:37 PM
 Author:	Victor
 Editor:	http://www.visualmicro.com
*/

#include "PowerMeter_CS5460Lib.h"

void PowerMeter_CS5460::initialize(int clkPin, int sdoPin)
{
	_clkPin = clkPin;
	_sdoPin = sdoPin;

	voltage = 0;
	current = 0;
	truePower = 0;
	powerFactor = 0;
	lostDataCount = 0;

	_sniffRegister = 0;
	_syncPulse = false;
	_readyReceived = false;
	_dataBuf = RingBuf_new(sizeof(uint32_t), BUFSIZE);

	// Setting up interrupt ISR on D2 (INT0), trigger function "clockISR()" when INT0 (CLK) is rising
	attachInterrupt(digitalPinToInterrupt(_clkPin), clockISR, RISING);

	// Set the CLK-pin and MISO-pin as inputs
	pinMode(_clkPin, INPUT);
	pinMode(_sdoPin, INPUT);

	// Setup timer0 interrupt
	noInterrupts();
	timer0_isr_init();
	timer0_attachInterrupt(timerOVF);
	resetTimer();
	interrupts();
}

void inline PowerMeter_CS5460::resetTimer()
{
	uint32_t offset = (clockCyclesPerMicrosecond() * TIMER_RESET_US);  // Converts microseconds to ticks
	timer0_write((ESP.getCycleCount() + offset));
}

inline void PowerMeter_CS5460::timerOVF()
{
	// Went ~20ms without a clock pulse, probably in an inter-frame period; reset sync
	_syncPulse = true;
	_readyReceived = false;
	_bitsForNextData = 64;
	resetTimer();
}

inline void PowerMeter_CS5460::clockISR()
{
	PowerMeter_CS5460::resetTimer();

	if (!_syncPulse && !_readyReceived) {
		return;
	}

	// Shift one bit to left and store MISO-value (0 or 1)
	_sniffRegister = (_sniffRegister << 1) | digitalRead(_sdoPin);
	_bitsForNextData--;

	if (_bitsForNextData == 0) {
		if (!_readyReceived) {
			// Check Status register is has conversion ready ( DRDY=1, ID=15 )
			_readyReceived = _sniffRegister == 0x009003C1;
			_bitsForNextData = 64;
		}
		else {
			_syncPulse = false;
			_bitsForNextData = 32;

			// Useful data. Save in data buffer
			if (_dataBuf->add(_dataBuf, (void*)&_sniffRegister) < 0)
			{
				lostDataCount++;
			}
		}
	}
}

bool PowerMeter_CS5460::update()
{
	bool frameAvailable = _dataBuf->numElements(_dataBuf) >= REGISTERS_PER_FRAME;

	if (frameAvailable)
	{
		uint32_t rawRegister;

		// read Vrms register
		_dataBuf->pull(_dataBuf, &rawRegister);
		voltage = rawRegister * VOLTAGE_MULTIPLIER;

		// read Irms register
		_dataBuf->pull(_dataBuf, &rawRegister);
		current = rawRegister * CURRENT_MULTIPLIER;

		// read E (energy) register
		_dataBuf->pull(_dataBuf, &rawRegister);
		if (rawRegister & 0x800000) {
			// must sign extend int24 -> int32LE
			rawRegister |= 0xFF000000;
		}
		truePower = ((int32_t)rawRegister) * POWER_MULTIPLIER;

		float apparent_power = voltage * current;
		powerFactor = truePower / apparent_power;
	}

	return frameAvailable;
}