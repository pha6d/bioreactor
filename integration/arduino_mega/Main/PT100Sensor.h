/*
 * PT100Sensor.h
 * This file defines a class for reading temperature from a PT100 sensor using the MAX31865 amplifier.
 * The class implements the SensorInterface and provides methods to initialize the sensor and read the temperature.
 *
 * Physical modules used:
 * - Adafruit MAX31865 RTD PT100 Amplifier
 * - PT100 Temperature Sensor
 */

#ifndef PT100SENSOR_H
#define PT100SENSOR_H

#include "SensorInterface.h"
#include <Adafruit_MAX31865.h>

class PT100Sensor : public SensorInterface {
public:
    /*
     * Constructor for PT100Sensor.
     * @param csPin: The chip select pin connected to the MAX31865.
     * @param diPin: The data input pin (MOSI) connected to the MAX31865.
     * @param doPin: The data output pin (MISO) connected to the MAX31865.
     * @param clkPin: The clock pin connected to the MAX31865.
     */
    PT100Sensor(int csPin, int diPin, int doPin, int clkPin);

    /*
     * Method to initialize the PT100 sensor.
     */
    void begin();

    /*
     * Method to read the temperature value from the sensor.
     * @return: The temperature in degrees Celsius.
     */
    float readValue();

private:
    int _csPin;    // Chip select pin
    int _diPin;    // Data input pin (MOSI)
    int _doPin;    // Data output pin (MISO)
    int _clkPin;   // Clock pin
    Adafruit_MAX31865 _max31865; // MAX31865 amplifier object
};

#endif
