#ifndef TEST_ACTUATORS_AND_SENSORS_H
#define TEST_ACTUATORS_AND_SENSORS_H

#include "ActuatorInterface.h"
#include "SensorInterface.h"
#include "PeristalticPump.h"
#include "DCPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"
#include "PHSensor.h"
#include "OxygenSensor.h"
#include "TurbiditySensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PT100Sensor.h"
#include "AirFlowSensor.h"

class TestActuatorsAndSensorsProgram {
public:
    void begin(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor, PeristalticPump& nutrientPump, PeristalticPump& basePump, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight, PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor);
    void update();
    bool isRunning();
private:
    DCPump* airPump;
    DCPump* drainPump;
    StirringMotor* stirringMotor;
    PeristalticPump* nutrientPump;
    PeristalticPump* basePump;
    HeatingPlate* heatingPlate;
    LEDGrowLight* ledGrowLight;
    PT100Sensor* waterTempSensor;
    DS18B20TemperatureSensor* airTempSensor;
    PHSensor* phSensor;
    TurbiditySensor* turbiditySensor;
    OxygenSensor* oxygenSensor;
    AirFlowSensor* airFlowSensor;

    bool running;
    unsigned long startTime;
};

#endif
