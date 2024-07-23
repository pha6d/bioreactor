#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

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
#include "DrainProgram.h"
#include "MixProgram.h"
#include "TestActuatorsProgram.h"
#include "FermentationProgram.h"
#include "Logger.h"
#include "PIDManager.h"
#include "VolumeManager.h"

enum class ProgramState {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED,
    ERROR
};

class StateMachine {
public:
    StateMachine(Logger& logger, PIDManager& pidManager, VolumeManager& volumeManager);

    void update(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                PeristalticPump& basePump, StirringMotor& stirringMotor,
                HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);

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

    void startTemperaturePID(double setpoint);
    void startPHPID(double setpoint);
    void startDOPID(double setpoint);
    
    void stopCurrentProgram();
    void stopAll(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                 PeristalticPump& basePump, StirringMotor& stirringMotor,
                 HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);

    String getCurrentProgram() const;
    String getCurrentStatus() const;
    ProgramState getState() const { return currentState; }

private:
    ProgramState currentState;
    String currentProgram;
    Logger& logger;
    VolumeManager& volumeManager;

    DrainProgram drainProgram;
    MixProgram mixProgram;
    TestActuatorsProgram testProgram;
    PIDManager& pidManager;
    FermentationProgram fermentationProgram;

    unsigned long startTime;

    void updateStateAndProgram();
};

#endif // STATE_MACHINE_H