#ifndef FERMENTATION_PROGRAM_H
#define FERMENTATION_PROGRAM_H

#include "ProgramBase.h"
#include "PIDManager.h"
#include "VolumeManager.h"
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

class FermentationProgram : public ProgramBase {
public:
    FermentationProgram(PIDManager& pidManager, VolumeManager& volumeManager);

    void configure(DCPump& airPump, DCPump& drainPump,
               PeristalticPump& nutrientPump, PeristalticPump& basePump,
               StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
               PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
               PHSensor& phSensor, TurbiditySensor& turbiditySensor,
               OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
               float tempSetpoint, float phSetpoint, float doSetpoint,
               float nutrientConc, float baseConc, int duration,
               const String& experimentName, const String& comment);

    void begin() override;

    // Méthodes héritées de ProgramBase
    void update() override;
    void pause() override;
    void resume() override;
    void stop() override;
    bool isRunning() const override;
    String getName() const override { return "Fermentation"; }

private:
    PIDManager& pidManager;
    VolumeManager& volumeManager;

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

    // Fermentation parameters
    float tempSetpoint;
    float phSetpoint;
    float doSetpoint;
    float nutrientConc;
    float baseConc;
    int duration;
    String experimentName;
    String comment;

    unsigned long startTime;
    unsigned long pauseStartTime;
    unsigned long totalPauseTime;
    bool running;
    bool paused;

    void updateSensors();
    void applyPIDOutputs();
    void updateVolume();
    void checkCompletion();
};

#endif // FERMENTATION_PROGRAM_H