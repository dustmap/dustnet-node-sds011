/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

#include <EEPROM.h>
#include <SPI.h>
// #include <LowPower.h>
#include <lmic.h>
#include <hal/hal.h>

#include "config.h"
#include "utils.h"

#include "SDS011.h"
#include "DHT22.h"

SDS011 sensorSDS011(SDS011_PIN_RX, SDS011_PIN_TX, MEASURE_INTERVAL);
DHT22 sensorDHT22(DHT22_PIN, MEASURE_INTERVAL);

static osjob_t sendjob;

static u1_t devEUI[8] = DEVEUI;
void os_getDevEui (u1_t* buf) {
	memcpy(buf, devEUI, 8);
}

static const u1_t PROGMEM appEUI[8] = APPEUI;
void os_getArtEui (u1_t* buf) {
	memcpy_P(buf, appEUI, 8);
}

static const u1_t PROGMEM appKey[16] = APPKEY;
void os_getDevKey (u1_t* buf) {
	memcpy_P(buf, appKey, 16);
}

const lmic_pinmap lmic_pins = {
    .nss = LMIC_PIN_NSS,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = LMIC_UNUSED_PIN,
    .dio = LMIC_PIN_DIO,
};

void onEvent (ev_t ev) {
    Sprint(os_getTime() + ": ");

    switch(ev) {
        case EV_SCAN_TIMEOUT:
            Sprintln(F("EV_SCAN_TIMEOUT"));
            break;
        case EV_BEACON_FOUND:
            Sprintln(F("EV_BEACON_FOUND"));
            break;
        case EV_BEACON_MISSED:
            Sprintln(F("EV_BEACON_MISSED"));
            break;
        case EV_BEACON_TRACKED:
            Sprintln(F("EV_BEACON_TRACKED"));
            break;
        case EV_JOINING:
            Sprintln(F("EV_JOINING"));
            break;
        case EV_JOINED:
            Sprintln(F("EV_JOINED"));

			// Set spreading factor to maximise range (SF12)
			LMIC_setDrTxpow(DR_SF12, 14);

            // Disable link check validation (automatically enabled
            // during join, but not supported by TTN at this time).
            LMIC_setLinkCheckMode(0);
            break;
        case EV_RFU1:
            Sprintln(F("EV_RFU1"));
            break;
        case EV_JOIN_FAILED:
            Sprintln(F("EV_JOIN_FAILED"));
            break;
        case EV_REJOIN_FAILED:
            Sprintln(F("EV_REJOIN_FAILED"));
            break;
        case EV_TXCOMPLETE:
            Sprintln(F("EV_TXCOMPLETE (includes waiting for RX windows)"));
            if (LMIC.txrxFlags & TXRX_ACK)
              Sprintln(F("Received ack"));
            if (LMIC.dataLen) {
              Sprintln(F("Received "));
              Sprintln(LMIC.dataLen);
              Sprintln(F(" bytes of payload"));
            }

            // Send the arduino to sleep until next transmission
            // for (int i=0; i<(int)(TX_INTERVAL/8); i++) {
            //     LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            // }
            // do_send(&sendjob);
             os_setTimedCallback(&sendjob, os_getTime()+sec2osticks(TX_INTERVAL), do_send);
            break;
        case EV_LOST_TSYNC:
            Sprintln(F("EV_LOST_TSYNC"));
            break;
        case EV_RESET:
            Sprintln(F("EV_RESET"));
            break;
        case EV_RXCOMPLETE:
            // data received in ping slot
            Sprintln(F("EV_RXCOMPLETE"));
            break;
        case EV_LINK_DEAD:
            Sprintln(F("EV_LINK_DEAD"));
            break;
        case EV_LINK_ALIVE:
            Sprintln(F("EV_LINK_ALIVE"));
            break;
         default:
            Sprintln(F("Unknown event"));
            break;
    }
}

void do_send(osjob_t* j) {
    if (LMIC.opmode & OP_TXRXPEND) {
        Sprintln(F("OP_TXRXPEND, not sending"));
    } else {
		// TODO batteryValue is not yet implemented; reserved for future use.
        uint8_t batteryValue = 0;

        byte deviceId[2];
		int16_t sensorPM25[SAMPLE_COUNT], sensorPM10[SAMPLE_COUNT];
        int sensorHumidity[SAMPLE_COUNT], sensorTemperature[SAMPLE_COUNT];

        sensorSDS011.wakeup();
        for (uint8_t i = 0; i < SAMPLE_COUNT; i++) {
			digitalWrite(13, HIGH);

    		bool newValueAvailable = false;
        	while (!newValueAvailable) {
				newValueAvailable = sensorSDS011.read(&sensorPM25[i], &sensorPM10[i], deviceId);
			}

    		Sprintln(" - SDS011 P2.5(" + String(i) + "): " + String(sensorPM25[i]) + "µg/m³");
			Sprintln(" - SDS011 PM10(" + String(i) + "):  " + String(sensorPM10[i]) + "µg/m³");

			newValueAvailable = false;
        	while(!newValueAvailable) {
    			newValueAvailable = sensorDHT22.read(&sensorHumidity[i], &sensorTemperature[i]);
    		}

			digitalWrite(13, LOW);

			Sprintln(" - DHT22 Humidity(" + String(i) + "): " + String(sensorHumidity[i] * 0.1) + "%");
			Sprintln(" - DHT22 Temperature(" + String(i) + "):  " + String(sensorTemperature[i] * 0.1) + "°C");
        }
		sensorSDS011.sleep();

		uint16_t medianSensorPM25 = median(SAMPLE_COUNT, sensorPM25);
		uint16_t medianSensorPM10 = median(SAMPLE_COUNT, sensorPM10);

		uint16_t medianSensorHumidity = median(SAMPLE_COUNT, sensorHumidity);
		uint16_t medianSensorTemperature = median(SAMPLE_COUNT, sensorTemperature);

        Sprintln("MEDIAN PM2.5: " + String(medianSensorPM25) + "µg/m³");
        Sprintln("MEDIAN PM10:  " + String(medianSensorPM10) + "µg/m³");

        Sprintln("MEDIAN Humidity:    " + String(medianSensorHumidity * 0.1) + "%");
        Sprintln("MEDIAN Temperature: " + String(medianSensorTemperature * 0.1) + "°C");

        byte payload[8];
        payload[0] = batteryValue;

        // Encode SDS011 P2.5 value
        payload[1] = highByte(medianSensorPM25);
        payload[2] = lowByte(medianSensorPM25);

        // Encode SDS011 PM10 value
		payload[3] = highByte(medianSensorPM10);
		payload[4] = lowByte(medianSensorPM10);

		payload[5] = lowByte((uint8_t)(medianSensorHumidity * 0.1));

		payload[6] = highByte(medianSensorTemperature);
		payload[7] = lowByte(medianSensorTemperature);

        LMIC_setTxData2(1, payload, sizeof(payload), 0);
        Sprintln(F("Packet queued"));
    }
}

void setup() {
    #ifdef ENABLE_DEBUG
    Serial.begin(9600);
    #endif

    Sprintln(F("Starting"));
	pinMode(13, OUTPUT);

	sensorSDS011.sleep();
	digitalWrite(13, HIGH);
    delay(4000);
	digitalWrite(13, LOW);

    // LMIC init
    os_init();

    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    // This fixes an issue with our 8MHZ controller and OTAA
    // See https://github.com/matthijskooijman/arduino-lmic/issues/22
    LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
    // Start job (sending automatically starts OTAA too)
    do_send(&sendjob);
}

void loop() {
    os_runloop_once();
}
