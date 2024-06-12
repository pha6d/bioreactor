#ifndef FERMENTATION_H
#define FERMENTATION_H

#include "DCPump.h"
#include "PeristalticPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"
#include "PT100Sensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PHSensor.h"
#include "TurbiditySensor.h"
#include "OxygenSensor.h"
#include "AirFlowSensor.h"

class FermentationProgram {
public:
    void begin(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
        PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
        float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc, float baseConc, int duration, const String& experimentName, const String& comment);
    void update();
    bool isRunning();
private:
    DCPump* airPump;
    DCPump* drainPump;
    PeristalticPump* nutrientPump;
    PeristalticPump* basePump;
    StirringMotor* stirringMotor;
    HeatingPlate* heatingPlate;
    LEDGrowLight* ledGrowLight;
    PT100Sensor* waterTempSensor;
    DS18B20TemperatureSensor* airTempSensor;
    PHSensor* phSensor;
    TurbiditySensor* turbiditySensor;
    OxygenSensor* oxygenSensor;
    AirFlowSensor* airFlowSensor;

    float tempSetpoint;
    float phSetpoint;
    float doSetpoint;
    float nutrientConc;
    float baseConc;
    int duration;
    String experimentName;
    String comment;

    unsigned long startTime;
    bool running;
};

#endif // FERMENTATION_H
