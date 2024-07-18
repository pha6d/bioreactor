/*
 * FermentationProgram.h
 * This file defines the FermentationProgram class which manages the fermentation process.
 * It coordinates actuators, sensors, and PID control for the bioreactor.
 */

#ifndef FERMENTATION_PROGRAM_H
#define FERMENTATION_PROGRAM_H

#include <Arduino.h>
#include "PIDManager.h"
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

extern bool stopFlag;

class FermentationProgram {
public:
    FermentationProgram(PIDManager& pidManager);

    void begin(DCPump& airPump, DCPump& drainPump, 
               PeristalticPump& nutrientPump, PeristalticPump& basePump, 
               StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
               PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, 
               PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
               OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
               float tempSetpoint, float phSetpoint, float doSetpoint, 
               float nutrientConc, float baseConc, int duration, 
               const String& experimentName, const String& comment);

    void update();
    bool isRunning() const;
    void stop();

private:
    PIDManager& pidManager;

    // Actuators
    DCPump* airPump;
    DCPump* drainPump;
    PeristalticPump* nutrientPump;
    PeristalticPump* basePump;
    StirringMotor* stirringMotor;
    HeatingPlate* heatingPlate;
    LEDGrowLight* ledGrowLight;

    // Sensors
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

    void updateSensors();
    void applyPIDOutputs();
    void checkCompletion();
};

#endif // FERMENTATION_PROGRAM_H