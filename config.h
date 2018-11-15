/* Copyright 2018 Dustmap.org. All rights reserved.
 *
 * This work is licensed under the terms of the MIT license.
 * For a copy, see <https://opensource.org/licenses/MIT>.
 */

// Enable feedback via Serial.print
//#define ENABLE_DEBUG

// Application EUI, LSB
// Should end with 0xD5, 0xB3, 0x70 for TTN
#ifndef APPEUI
#define APPEUI { 0x00, 0x00, 0x00, 0x00, 0x00, 0xD5, 0xB3, 0x70 }
#endif

// Device EUI, LSB
// Must be unique per application
#ifndef DEVEUI
#define DEVEUI { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#endif

// App Key, MSB
#ifndef APPKEY
#define APPKEY { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
#endif

// TX interval in seconds
// Might become longer due to duty cycle limitations.
#define TX_INTERVAL 600

// Number of samples taken for one measurement
#define SAMPLE_COUNT 11

// Measurement interval inbetween samples in seconds
#define MEASURE_INTERVAL 5

// Sensor SDS011 UART pin papping
#define SDS011_PIN_RX 8
#define SDS011_PIN_TX 9

// Sensor DHT22 pin mapping
#define DHT22_PIN 2

// HopeRF RFM95W pin mapping
// NSS & DIO0, DIO1, DIO2
#define LMIC_PIN_NSS 10
#define LMIC_PIN_DIO { 4, 5, 7 }
