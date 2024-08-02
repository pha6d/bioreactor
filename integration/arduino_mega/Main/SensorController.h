#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <Arduino.h>
#include "PT100Sensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PHSensor.h"
#include "TurbiditySensor.h"
#include "OxygenSensor.h"
#include "AirFlowSensor.h"

class SensorController {
public:
    static void initialize(PT100Sensor& waterTemp, DS18B20TemperatureSensor& airTemp,
                           PHSensor& ph, TurbiditySensor& turbidity,
                           OxygenSensor& oxygen, AirFlowSensor& airFlow);
    
    static float readSensor(const String& sensorName);
    static void updateAllSensors();
    static void beginAll();

private:
    static PT100Sensor* waterTempSensor;
    static DS18B20TemperatureSensor* airTempSensor;
    static PHSensor* phSensor;
    static TurbiditySensor* turbiditySensor;
    static OxygenSensor* oxygenSensor;
    static AirFlowSensor* airFlowSensor;

    static SensorInterface* findSensorByName(const String& name);
};

#endif // SENSOR_CONTROLLER_H