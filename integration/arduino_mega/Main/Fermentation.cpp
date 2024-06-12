#include "Fermentation.h"

// External variables for shared states
extern bool stopFlag;
extern String currentProgram;
extern String programStatus;

void FermentationProgram::begin(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
    float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc, float baseConc, int duration, const String& experimentName, const String& comment) {
    // Initialize the state
    currentProgram = "Fermentation";
    programStatus = "Running";
    running = true;

    // Store parameters and initialize actuators
    this->airPump = &airPump;
    this->drainPump = &drainPump;
    this->nutrientPump = &nutrientPump;
    this->basePump = &basePump;
    this->stirringMotor = &stirringMotor;
    this->heatingPlate = &heatingPlate;
    this->ledGrowLight = &ledGrowLight;
    this->waterTempSensor = &waterTempSensor;
    this->airTempSensor = &airTempSensor;
    this->phSensor = &phSensor;
    this->turbiditySensor = &turbiditySensor;
    this->oxygenSensor = &oxygenSensor;
    this->airFlowSensor = &airFlowSensor;

    this->tempSetpoint = tempSetpoint;
    this->phSetpoint = phSetpoint;
    this->doSetpoint = doSetpoint;
    this->nutrientConc = nutrientConc;
    this->baseConc = baseConc;
    this->duration = duration;
    this->experimentName = experimentName;
    this->comment = comment;
    this->startTime = millis();

    // Example: Start the air pump
    airPump.control(true, 50);
    Serial.println("Fermentation started: " + experimentName);
    Serial.println("Comment: " + comment);
}

void FermentationProgram::update() {
    if (!running) return;
    if (stopFlag) {
        programStatus = "Stopped";
        running = false;
        // Stop all actuators
        airPump->control(false, 0);
        drainPump->control(false, 0);
        nutrientPump->control(false, 0);
        basePump->control(false, 0);
        stirringMotor->control(false, 0);
        heatingPlate->control(false);
        ledGrowLight->control(false);
        Serial.println("Fermentation interrupted.");
        return;
    }

    // Update logic for fermentation process
    // Example: Check if the duration has passed
    if (millis() - startTime >= duration * 1000) {
        airPump->control(false, 0);
        running = false;
        programStatus = "Completed";
        Serial.println("Fermentation completed.");
    }
}

bool FermentationProgram::isRunning() {
    return running;
}
