/*
 * OxygenSensor.cpp
 * This file provides the implementation of the OxygenSensor class defined in OxygenSensor.h.
 * The class reads dissolved oxygen values from a dissolved oxygen sensor and performs calibration.
 */

#include "OxygenSensor.h"
#include <OneWire.h>

const uint16_t OxygenSensor::DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410
};

// Constructor for OxygenSensor
OxygenSensor::OxygenSensor(int pin, int temperaturePin) : _pin(pin), _temperaturePin(temperaturePin) {}

// Method to initialize the DO sensor
void OxygenSensor::begin() {
    // Nothing specific to initialize for DO sensor in this implementation
}

// Method to read the temperature from the DS18B20 sensor
float OxygenSensor::readTemperature() {
    byte data[12];
    byte addr[8];
    OneWire ds(_temperaturePin);

    if (!ds.search(addr)) {
        ds.reset_search();
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

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1); // Start temperature conversion

    delay(1000); // Wait for conversion to complete

    ds.reset();
    ds.select(addr);
    ds.write(0xBE); // Read Scratchpad

    for (int i = 0; i < 9; i++) {
        data[i] = ds.read();
    }

    ds.reset_search();

    int16_t rawTemperature = (data[1] << 8) | data[0];
    return rawTemperature / 16.0; // Convert raw temperature to Celsius
}

// Method to read the DO value from the sensor
float OxygenSensor::readValue() {
    uint16_t rawValue = analogRead(_pin); // Read the analog value from DO sensor
    uint16_t voltage = uint32_t(VREF) * rawValue / ADC_RES; // Convert ADC value to voltage
    float temperature = readTemperature();

    uint16_t V_saturation;
    if (TWO_POINT_CALIBRATION == 0) {
        V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature - (uint32_t)CAL1_T * 35;
    } else {
        V_saturation = (int16_t)((int8_t)temperature - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
    }

    return (voltage * DO_Table[(int)temperature] / V_saturation); // Calculate DO concentration
}

// Calibration
void OxygenSensor::calibrate() {
    uint32_t raw = analogRead(_pin);
    Serial.println("raw:\t" + String(raw) + "\tVoltage(mv):\t" + String(raw * VREF / ADC_RES));
}
