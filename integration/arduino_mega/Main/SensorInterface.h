/*
 * SensorInterface.h
 * This file defines an abstract interface for sensors, providing a standard way
 * to read values from various sensors in the project. This interface can be implemented
 * by any sensor class, such as a temperature sensor, humidity sensor, etc.
 */

#ifndef SENSORINTERFACE_H
#define SENSORINTERFACE_H

class SensorInterface {
public:
    /*
     * Pure virtual function to read the sensor value.
     * @return: The value read from the sensor.
     */
    virtual float readValue() = 0;
};

#endif
