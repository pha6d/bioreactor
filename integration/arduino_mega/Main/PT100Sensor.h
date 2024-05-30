/*
 * PT100Sensor.h
 * This file defines a class for reading temperature from a PT100 sensor using the MAX31865 module.
 * The class implements the SensorInterface and provides methods to initialize the sensor and read the temperature.
 *
 * Physical modules used:
 * - PT100 Temperature Sensor
 * - MAX31865 Amplifier Module
 */

#ifndef PT100SENSOR_H
#define PT100SENSOR_H

#include "SensorInterface.h"
#include <Adafruit_MAX31865.h>

class PT100Sensor : public SensorInterface {
public:
    /*
     * Constructor for PT100Sensor.
     * @param csPin: Chip Select pin for SPI communication.
     * @param diPin: Data Input (MOSI) pin for SPI communication.
     * @param doPin: Data Output (MISO) pin for SPI communication.
     * @param clkPin: Clock pin for SPI communication.
     */
    PT100Sensor(int csPin, int diPin, int doPin, int clkPin);

    /*
     * Method to initialize the PT100 sensor.
     */
    void begin();

    /*
     * Method to read the temperature from the sensor.
     * @return: The temperature in degrees Celsius.
     */
    float readValue();

private:
    Adafruit_MAX31865 _thermo; // MAX31865 sensor object
};

#endif
