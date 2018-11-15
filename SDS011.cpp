/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

#include "SDS011.h"

SDS011::SDS011(uint8_t pin_rx, uint8_t pin_tx, uint16_t max_interval) {
	_max_interval = max_interval * 1000;
	_lastreadtime = -1 * _max_interval;

	serial = new SoftwareSerial(pin_rx, pin_tx);
	serial->begin(9600);
}

bool SDS011::read(int *pm25, int *pm10, byte *deviceId, bool force) {
	uint32_t currenttime = millis();
	if (!force && (currenttime-_lastreadtime) < _max_interval) {
		return false;
	}

	_lastreadtime = currenttime;

	int length = 0;
	int checksum = 0;

	while (serial->available() > 0) {
		byte buffer = serial->read();
		int value = static_cast<int>(buffer);

		switch (length) {
		// Message header (0xAA)
		case 0:
			if (buffer != 0xAA) {
				length = -1;
			}
			break;
		// Command ID (0xC0)
		case 1:
			if (buffer != 0xC0) {
				length = -1;
			}
			break;
		// DATA 1 (PM2.5 Low byte)
		case 2:
			*pm25 = value;
			checksum = value;
			break;
		// DATA 2 (PM2.5 High byte)
		case 3:
			*pm25 += value << 8;
			checksum += value;
			break;
		// DATA 3 (PM10 Low byte)
		case 4:
			*pm10 = value;
			checksum += value;
			break;
		// DATA 4 (PM10 High byte)
		case 5:
			*pm10 += value << 8;
			checksum += value;
			break;
		// DATA 5 (ID byte 1)
		case 6:
			deviceId[0] = buffer;
			checksum += value;
			break;
		// DATA 6 (ID byte 2)
		case 7:
			deviceId[1] = buffer;
			checksum += value;
			break;
		// Checksum byte
		// Low 8bit of the sum result of Data Bytes
		// (not including packet head, tail and Command ID)
		case 8:
			if (value != checksum % 256) {
				length = -1;
			}
			break;
		// Message tail (0xAB)
		case 9:
			if (buffer != 0xAB) {
				length = -1;
			}
			break;
		case 10:
			return true;
		}

		length++;
	}

	return false;
}

void SDS011::sleep() {
	static byte sleepBytes[19];
	sleepBytes[0] = 0xAA;
	sleepBytes[1] = 0xB4;
	sleepBytes[2] = 0x06;
	sleepBytes[3] = 0x01;
	sleepBytes[15] = 0xFF;
	sleepBytes[16] = 0xFF;
	sleepBytes[17] = 0x05; // Should be 0x08 acording to datasheet?
	sleepBytes[18] = 0xAB;

	for (uint8_t i = 0; i < 19; i++) {
		serial->write(sleepBytes[i]);
	}
	serial->flush();

	while (serial->available() > 0) {
		serial->read();
	}
}

void SDS011::wakeup() {
	serial->write(0x01);
	serial->flush();
}
