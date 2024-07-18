/*
 * BioReactorManager.h
 * This file defines the BioReactorManager class which coordinates the overall operation of the bioreactor.
 * It manages the interaction between different components such as the fermentation program, safety system, and logger.
 */

#ifndef BIOREACTORMANAGER_H
#define BIOREACTORMANAGER_H

#include "FermentationProgram.h"
#include "SafetySystem.h"
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

class BioReactorManager {
public:
    BioReactorManager(FermentationProgram& fermentationProgram, 
                      SafetySystem& safetySystem,
                      Logger& logger,
                      PIDManager& pidManager,
                      DCPump& airPump,
                      DCPump& drainPump,
                      PeristalticPump& nutrientPump,
                      PeristalticPump& basePump,
                      StirringMotor& stirringMotor,
                      HeatingPlate& heatingPlate,
                      LEDGrowLight& ledGrowLight,
                      PT100Sensor& waterTempSensor,
                      DS18B20TemperatureSensor& airTempSensor,
                      PHSensor& phSensor,
                      TurbiditySensor& turbiditySensor,
                      OxygenSensor& oxygenSensor,
                      AirFlowSensor& airFlowSensor);

    void initialize();
    void update();
    void startFermentation(float tempSetpoint, float phSetpoint, float doSetpoint,
                           float nutrientConc, float baseConc, int duration,
                           const String& experimentName, const String& comment,
                           float totalVolume, float maxVolumePercent, float minVolume);
    void stopFermentation();
    void handleCommand(const String& command);

    void testTemperaturePID(float targetTemperature);
    void testPHPID(float targetPH);
    void testDOPID(float targetDO);

    void manuallyAdjustVolume(float volume, const String& source);

private:
    FermentationProgram& fermentationProgram;
    SafetySystem& safetySystem;
    Logger& logger;
    PIDManager& pidManager;

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

    float currentVolume;
    float totalVolume;
    float maxVolumePercent;
    float minVolume;
    float addedNaOH;
    float addedNutrient;
    float addedMicroalgae;
    float removedVolume;

    void checkSafetyLimits();
    void logData();
    void adjustStirringSpeed();
    void updateVolume();
};

#endif // BIOREACTORMANAGER_H