/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

#include <Arduino.h>
#include <SoftwareSerial.h>

class SDS011 {
	public:
		SDS011(uint8_t pin_rx, uint8_t pin_tx, uint16_t max_interval = 2000);
		bool read(int *pm25, int *pm10, byte *deviceId, bool force = false);
		void sleep();
		void wakeup();

	private:
		uint32_t _lastreadtime;
		uint16_t _max_interval;
		SoftwareSerial *serial;
};
