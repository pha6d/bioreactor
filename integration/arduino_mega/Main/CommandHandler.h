#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include "StateMachine.h"
#include "SafetySystem.h"
#include "VolumeManager.h"
#include "Logger.h"
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

class CommandHandler {
public:
    CommandHandler(StateMachine& stateMachine, SafetySystem& safetySystem, 
                   VolumeManager& volumeManager, Logger& logger,
                   DCPump& airPump, DCPump& drainPump, 
                   PeristalticPump& nutrientPump, PeristalticPump& basePump,
                   StirringMotor& stirringMotor, HeatingPlate& heatingPlate, 
                   LEDGrowLight& ledGrowLight, PT100Sensor& waterTempSensor, 
                   DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, 
                   TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, 
                   AirFlowSensor& airFlowSensor, PIDManager& pidManager);

    void executeCommand(const String& command);

private:
    StateMachine& stateMachine;
    SafetySystem& safetySystem;
    VolumeManager& volumeManager;
    Logger& logger;
    DCPump& airPump;
    DCPump& drainPump;
    PeristalticPump& nutrientPump;
    PeristalticPump& basePump;
    StirringMotor& stirringMotor;
    HeatingPlate& heatingPlate;
    LEDGrowLight& ledGrowLight;
    PT100Sensor& waterTempSensor;
    DS18B20TemperatureSensor& airTempSensor;
    PHSensor& phSensor;
    TurbiditySensor& turbiditySensor;
    OxygenSensor& oxygenSensor;
    AirFlowSensor& airFlowSensor;
    PIDManager& pidManager;

    void printHelp();
    void parseFermentationParams(const String& command, float& tempSetpoint, float& phSetpoint, 
                                 float& doSetpoint, float& nutrientConc, float& baseConc, 
                                 int& duration, String& experimentName, String& comment);
};

#endif // COMMAND_HANDLER_H