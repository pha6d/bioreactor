/*
 * TemperatureSensor.cpp
 * This file provides the implementation of the TemperatureSensor class defined in TemperatureSensor.h.
 * The class reads temperature from the DS18B20 sensor.
 */

#include "TemperatureSensor.h"

// Constructor for TemperatureSensor
TemperatureSensor::TemperatureSensor(int pin) : _ds(pin), _pin(pin) {}

// Method to initialize the temperature sensor
void TemperatureSensor::begin() {
    // Nothing specific to initialize for DS18B20 in this implementation
}

// Method to read the temperature from the sensor
float TemperatureSensor::readValue() {
    byte data[12];
    byte addr[8];

    if (!_ds.search(addr)) {
        _ds.reset_search();
        return -1000; // Return an error value if no sensor found
    }

    if (OneWire::crc8(addr, 7) != addr[7]) {
        Serial.println("CRC is not valid!");
        return -1000; // Return an error value if CRC check fails
    }

    if (addr[0] != 0x10 && addr[0] != 0x28) {
        Serial.print("Device is not recognized");
        return -1000; // Return an error value if the device is not recognized
    }

    _ds.reset();
    _ds.select(addr);
    _ds.write(0x44, 1); // Start temperature conversion

    delay(1000); // Wait for conversion to complete

    _ds.reset();
    _ds.select(addr);
    _ds.write(0xBE); // Read Scratchpad

    for (int i = 0; i < 9; i++) {
        data[i] = _ds.read();
    }

    _ds.reset_search();

    int16_t rawTemperature = (data[1] << 8) | data[0];
    return rawTemperature / 16.0; // Convert raw temperature to Celsius
}
