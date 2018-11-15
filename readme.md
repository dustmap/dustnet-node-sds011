# Dustnet Node

This is the firmware for a dustnet-node. It has been build especially for the [Dustmap](https://dustmap.org) project. It uses [RoLaWAN](https://lora-alliance.org/about-lorawan) and [The Things Network](https://www.thethingsnetwork.org) (short: TTN) to transmit sensor measurements.

*This documentation is a work-in-progress.*

This Project has been generously supported by [![netidee](https://www.netidee.at/themes/Netidee/images/netidee-logo-color.svg)](https://www.netidee.at).

## Hardware

A dustmap-node is usually equipped with an air quality sensor ([SDS011](https://aqicn.org/sensor/sds011)) and a temperature and humidity sensor (DHT22).

## Software

The firmware can be installed on most Arduino-compatible devices. For the Dustmap-project we use a custom board based on the Arduino Pro Mini.

### Configuration

The configuration can be found in the `config.h` file. Please be advised that we do not recommend to change anything in here. Except you know exactly, what you are doing. Than you will properly be fine.

| Key | Description |
| --- | --- |
| `ENABLE_DEBUG` | Enables debug output via serial interface. This will significantly increase the filesize. *Uncomment to enable* |
| `APPEUI` | The Things Network Application EUI. See `The Things Network Integration` for more information. |
| `DEVEUI` | The Things Network Device EUI. See `The Things Network Integration` for more information. |
| `APPKEY` | The Things Network Application Key. See `The Things Network Integration` for more information. |
| `TX_INTERVAL` | Delay in seconds between measurements. *Default: 600* |
| `MEASSURE_INTERVAL` | Delay in seconds between samples of one measurement. *Default: 5* |
| `SAMPLE_COUNT` | Number of samples taken during one measurement. *Default: 11* |
| `SDS011_PIN_RX` | Pin mapping for RX-pin of the SDS011 *Default: 8* |
| `SDS011_PIN_TX` | Pin mapping for TX-pin of the SDS011 *Default: 9* |
| `DHT22_PIN` | Pin mapping for the DHT22 *Default: 2* |
| `LMIC_PIN_NSS` |  |
| `LMIC_PIN_DIO` |  |

To compile this code, you need to have the [Arduino-LMIC library](https://github.com/matthijskooijman/arduino-lmic) by [Matthijs Kooijman](https://github.com/matthijskooijman) installed. We recommend to check it out directly from Github as using "Manage Libraries..." in the Arduino IDE will most likely fail.

### The Things Network Integration

To contribute data to the dustmap project, you need to obtain `DevEUI`, `AppEUI` and `AppKey` from the dustmap API. See *Expert-Mode* if you want to use this firmware with your own TTN application.

<details>
  <summary>Expert-Mode: Setup your own The Things Network Application</summary>
  To use this firmware in Expert-Mode you have to setup your own TTN-application.

  #### Decoder & Converter
  
  Sensor values are converted into int and packed into bytes. The following functions will help you to convert them back into useful values. If you plan on using your node with the provided AppEUI and AppKey, you can safely ignore this part.

  Payload decoder function for the TTN applications:

  ```
  function Decoder(bytes, port) {
      // Decode bytes to number
      // - First byte: Battery State
      // - Second + Third byte: PM2.5 Sensor value
      // - Fourth + Fifth byte: PM10 Sensor value
      // - sixth + seventh byte: Humidity & Temperature Sensor value

      var batteryInt = bytes[0];
      var sensorPM25Int = (bytes[1] << 8) | bytes[2];
      var sensorPM10Int = (bytes[3] << 8) | bytes[4];
      var sensorHumidityInt = bytes[5] & 0x7F;
      var sensorTemperatureInt = ((bytes[5] >> 7) & 0x1) | (bytes[6] << 1);

      return {
          battery: batteryInt,
          sensorPM25: sensorPM25Int,
          sensorPM10: sensorPM10Int,
          sensorHumidity: sensorHumidityInt,
          sensorTemperature: sensorTemperatureInt,
      };
  }
  ```

  Payload converter function for the TTN applications:


  ```
  function Converter(decoded, port) {
      decoded.sensorPM25 = Math.round(decoded.sensorPM25 / 10);
      decoded.sensorPM10 = Math.round(decoded.sensorPM10 / 10);

      // Map temperature from 0-512 to -40-80°C
    	decoded.sensorTemperature = Math.round((decoded.sensorTemperature - 0) * (800 - (-400)) / (512 - 0) + (-400));
    	decoded.sensorTemperature /= 10;

    	return decoded;
  }
  ```
</details>



## Licence

Copyright 2018 Dustmap.org. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, see <https://opensource.org/licenses/MIT>.
