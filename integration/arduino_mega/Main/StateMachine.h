#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <Arduino.h>
#include "ProgramBase.h"
#include "Logger.h"
#include "PIDManager.h"
#include "VolumeManager.h"
#include "DrainProgram.h"
#include "MixProgram.h"
#include "FermentationProgram.h"
#include "PIDTestProgram.h"
#include "TestActuatorsProgram.h"
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

enum class ProgramState {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED,
    STOPPED,
    ERROR
};

class StateMachine {
public:
    StateMachine(Logger& logger, PIDManager& pidManager, VolumeManager& volumeManager);
    void addProgram(ProgramBase* program);
    void update();
    void startProgram(const String& programName);
    void stopProgram(const String& programName);
    void stopAllPrograms();
    ProgramState getCurrentState() const;
    String getCurrentProgram() const;

    // Configuration methods
    void configureDrain(DCPump& drainPump, int rate, int duration);
    void configureMix(StirringMotor& stirringMotor, int speed);
    void configureFermentation(DCPump& airPump, DCPump& drainPump,
                               PeristalticPump& nutrientPump, PeristalticPump& basePump,
                               StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                               PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                               PHSensor& phSensor, TurbiditySensor& turbiditySensor,
                               OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
                               float tempSetpoint, float phSetpoint, float doSetpoint,
                               float nutrientConc, float baseConc, int duration,
                               const String& experimentName, const String& comment);
    void configurePIDTest(const String& pidType, double setpoint);

    // Start methods
    void startDrain(DCPump& drainPump, int rate, int duration);
    void startMix(StirringMotor& stirringMotor, int speed);
    void startTests(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor, 
                    PeristalticPump& nutrientPump, PeristalticPump& basePump, 
                    HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                    PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
                    OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor);
    void startFermentation(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, 
                           PeristalticPump& basePump, StirringMotor& stirringMotor, 
                           HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                           PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                           PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
                           OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
                           float tempSetpoint, float phSetpoint, float doSetpoint,
                           float nutrientConc, float baseConc, int duration,
                           const String& experimentName, const String& comment);
    void startPIDTest(const String& pidType, double setpoint);
    void startActuatorTest(const String& actuatorName, float value, int duration);

    void stopAll(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                 PeristalticPump& basePump, StirringMotor& stirringMotor,
                 HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);
    void startSensorTest();

private:
    static const int MAX_PROGRAMS = 10;
    ProgramBase* programs[MAX_PROGRAMS];
    int programCount;
    ProgramState currentState;
    ProgramBase* currentProgram;
    Logger& logger;
    PIDManager& pidManager;
    VolumeManager& volumeManager;

    void transitionToState(ProgramState newState);
};

#endif // STATE_MACHINE_H