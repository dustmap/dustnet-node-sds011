/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

#include <Arduino.h>

class DHT22 {
	public:
		DHT22(uint8_t pin_data, uint16_t max_interval = 2000);
		bool read(int *humidity, int *temperature, bool force = false);

	private:
		 uint8_t _pin_data, _bit, _port;
		 uint16_t _max_interval;
		 uint32_t _lastreadtime, _maxcycles;
		 uint32_t expectPulse(bool level);
};
