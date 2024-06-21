#include "StateMachine.h"
#include "TestActuatorsAndSensors.h"
#include "Fermentation.h"
#include "Drain.h"
#include "Mix.h"
#include "Logger.h" // Include Logger

// External variables for shared states
extern String currentProgram;
extern String programStatus;

StateMachine::StateMachine()
    : currentState(IDLE), startTime(0) {}

void StateMachine::startDrain(DCPump& drainPump, int rate, int duration) {
    Serial.println("Starting drain process...");
    drainProgram.begin(drainPump, rate, duration);
    logStartupParameters("Drain", rate, duration, 0, 0, 0, 0, 0, "Drain", "Starting drain program");
    currentState = RUNNING;
}

void StateMachine::startMix(StirringMotor& stirringMotor, int speed) {
    Serial.println("Starting mix process...");
    mixProgram.begin(stirringMotor, speed);
    logStartupParameters("Mix", speed, 0, 0, 0, 0, 0, 0, "Mix", "Starting mix program");
    currentState = RUNNING;
}

void StateMachine::startTests(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor,
    PeristalticPump& nutrientPump, PeristalticPump& basePump,
    HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
    PHSensor& phSensor, TurbiditySensor& turbiditySensor,
    OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor) {
    Serial.println("Starting tests...");
    testProgram.begin(airPump, drainPump, stirringMotor, nutrientPump, basePump, heatingPlate, ledGrowLight,
        waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
    currentState = RUNNING;
}

void StateMachine::startFermentation(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
    PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate,
    LEDGrowLight& ledGrowLight, PT100Sensor& waterTempSensor,
    DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor,
    TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor,
    AirFlowSensor& airFlowSensor, float tempSetpoint, float phSetpoint,
    float doSetpoint, float nutrientConc, float baseConc, int duration,
    const String& experimentName, const String& comment) {
    Serial.println("Starting fermentation process...");
    fermentationProgram.begin(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
        waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
        tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, experimentName, comment);
    logStartupParameters("Fermentation", 0, duration, tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, experimentName, comment); // Log the startup parameters
    currentState = RUNNING;
}

void StateMachine::stopAll(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
    PeristalticPump& basePump, StirringMotor& stirringMotor,
    HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight) {
    airPump.control(false, 0);
    drainPump.control(false, 0);
    nutrientPump.control(false, 0);
    basePump.control(false, 0);
    stirringMotor.control(false, 0);
    heatingPlate.control(false, 0);
    ledGrowLight.control(false);
    currentState = STOPPING;
    programStatus = "Stopped";  // Update the program status
    Serial.println("Stopping all systems.");
}

void StateMachine::update(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                          PeristalticPump& basePump, StirringMotor& stirringMotor,
                          HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight) {
    switch (currentState) {
        case RUNNING:
            if (drainProgram.isRunning()) {
                drainProgram.update();
            }
            if (mixProgram.isRunning()) {
                mixProgram.update();
            }
            if (testProgram.isRunning()) {
                testProgram.update();
            }
            if (fermentationProgram.isRunning()) {
                fermentationProgram.update();
            }
            break;
        case STOPPING:
            if (!airPump.isOn() && !drainPump.isOn() && !nutrientPump.isOn() &&
                !basePump.isOn() && !stirringMotor.isOn() && !heatingPlate.isOn() &&
                !ledGrowLight.isOn()) {
                currentState = IDLE;
                Serial.println("All systems stopped.");
                programStatus = "Stopped"; // Ensure status is updated to Stopped
            }
            break;
        default:
            break;
    }
}

