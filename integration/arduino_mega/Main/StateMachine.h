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

// Enum to represent different states of the bioreactor
enum class ProgramState {
    IDLE,
    RUNNING,
    PAUSED,
    COMPLETED,
    ERROR
};

class StateMachine {
public:
    // Constructor
    StateMachine(Logger& logger, PIDManager& pidManager, VolumeManager& volumeManager);

    // Main update function to be called in the loop
    void update(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                PeristalticPump& basePump, StirringMotor& stirringMotor,
                HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);

    // Functions to start different programs
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

    // New functions for individual PID control
    void startTemperaturePID(double setpoint);
    void startPHPID(double setpoint);
    void startDOPID(double setpoint);
    void stopTemperaturePID();
    void stopPHPID();
    void stopDOPID();

    // Function to stop all processes
    void stopAll(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                 PeristalticPump& basePump, StirringMotor& stirringMotor,
                 HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);

    // Functions to get current state information
    String getCurrentProgram() const;
    String getCurrentStatus() const;
    ProgramState getState() const { return currentState; }

    // Function to stop all tests
    void stopAllTests();

    // Function to check if a test is running
    bool isTestRunning() const;

private:
    ProgramState currentState;
    String currentProgram;
    Logger& logger;
    VolumeManager& volumeManager;

    // Program objects
    DrainProgram drainProgram;
    MixProgram mixProgram;
    TestActuatorsProgram testProgram;
    PIDManager& pidManager;
    FermentationProgram fermentationProgram;

    bool testRunning;
    unsigned long startTime;

    // Private helper function to update state and program
    void updateStateAndProgram();
};

#endif // STATE_MACHINE_H