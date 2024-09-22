#ifndef SENSOR_CONTROLLER_H
#define SENSOR_CONTROLLER_H

#include <sensors.h>
#include <Arduino.h>
#include <logger/Logger.h>

class SensorController {
public:
    static void initialize(PT100Sensor& waterTemp, DS18B20TemperatureSensor& airTemp, DS18B20TemperatureSensor& electronicTempSensor,
                           PHSensor& ph,
                           OxygenSensor& oxygen, 
                           AirFlowSensor& airFlow, 
                           TurbiditySensorSEN0554& turbiditySEN0554);
    
    static float readSensor(const String& sensorName);
    static void updateAllSensors();
    static void beginAll();
    
    static SensorInterface* findSensorByName(const String& name);
    static void logSensorData();

private:
    static PT100Sensor* waterTempSensor;
    static DS18B20TemperatureSensor* airTempSensor;
    static DS18B20TemperatureSensor* electronicTempSensor;
    static PHSensor* phSensor;
    static OxygenSensor* oxygenSensor;
    static AirFlowSensor* airFlowSensor;
    static TurbiditySensorSEN0554* turbiditySensorSEN0554;
    
};

#endif // SENSOR_CONTROLLER_H