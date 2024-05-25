/*
 * TurbiditySensor.h
 * This file defines a class for reading turbidity values from a turbidity sensor.
 * The class implements the SensorInterface and provides methods to initialize the sensor and read the turbidity value.
 *
 * Physical modules used:
 * - Gravity: Analog Turbidity Sensor
 */

#ifndef TURBIDITYSENSOR_H
#define TURBIDITYSENSOR_H

#include "SensorInterface.h"
#include <Arduino.h>

class TurbiditySensor : public SensorInterface {
public:
    /*
     * Constructor for TurbiditySensor.
     * @param pin: The analog pin connected to the turbidity sensor.
     */
    TurbiditySensor(int pin);

    /*
     * Method to initialize the turbidity sensor.
     */
    void begin();

    /*
     * Method to read the turbidity value from the sensor.
     * @return: The turbidity voltage.
     */
    float readValue();

private:
    int _pin; // Analog pin connected to the turbidity sensor
};

#endif


/*
Installation Instructions:

For the Gravity: Analog Turbidity Sensor
@https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189
1. Connect the sensor output to an analog pin on the Arduino (A2 in this case).
2. Connect V+ to 5V on the Arduino and GND to GND.
*/

