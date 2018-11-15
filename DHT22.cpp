/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

#include "DHT22.h"

DHT22::DHT22(uint8_t pin_data, uint16_t max_interval) {
	_pin_data = pin_data;
	_bit = digitalPinToBitMask(pin_data);
	_port = digitalPinToPort(pin_data);
	_max_interval = max_interval * 1000;
	_lastreadtime = -1 * _max_interval;
	_maxcycles = microsecondsToClockCycles(1000);

	pinMode(_pin_data, INPUT_PULLUP);
}

bool DHT22::read(int *humidity, int *temperature, bool force) {
	uint32_t currenttime = millis();
	if (!force && (currenttime-_lastreadtime) < _max_interval) {
		return false;
	}

	_lastreadtime = currenttime;

	uint8_t data[5];
	uint32_t cycles[80];

	digitalWrite(_pin_data, HIGH);
	delay(250);

	pinMode(_pin_data, OUTPUT);
	digitalWrite(_pin_data, LOW);
	delay(20);

	noInterrupts();

	digitalWrite(_pin_data, HIGH);
	delayMicroseconds(40);

	pinMode(_pin_data, INPUT_PULLUP);
	delayMicroseconds(10);

	if (expectPulse(LOW) == 0) {
		return false;
	}

	if (expectPulse(HIGH) == 0) {
		return false;
	}

	for (int i=0; i<80; i+=2) {
		cycles[i] = expectPulse(LOW);
		cycles[i+1] = expectPulse(HIGH);
	}

	interrupts();

	for (int i=0; i<40; ++i) {
		uint32_t lowCycles = cycles[2*i];
		uint32_t highCycles = cycles[2*i+1];

		if (lowCycles == 0 || highCycles == 0) {
			return false;
		}

		data[i/8] <<= 1;
		if (highCycles > lowCycles) {
			data[i/8] |= 1;
		}
	}

	if ((data[0] + data[1] + data[2] + data[3]) & 0xFF != data[4]) {
		return false;
	}

	*humidity = data[0];
	*humidity *= 256;
	*humidity += data[1];

	*temperature = data[2] & 0x7F;
	*temperature *= 256;
	*temperature += data[3];
	if (data[2] & 0x80) {
		*temperature *= -1;
	}

	return true;
}

uint32_t DHT22::expectPulse(bool level) {
	uint32_t count = 0;

	uint8_t portState = level ? _bit : 0;
	while ((*portInputRegister(_port) & _bit) == portState) {
		if (count++ >= _maxcycles) {
			return 0;
		}
	}

	return count;
}
