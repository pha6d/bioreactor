#include "TestPIDControllers.h"

void TestPIDControllersProgram::beginTempPID(PID& tempPID, PT100Sensor& waterTempSensor) {
    currentPID = &tempPID;
    currentSensor = &waterTempSensor;
    this->waterTempSensor = &waterTempSensor;
    running = true;
    startTime = millis();
    Serial.println("Running Temperature PID controller test...");
}

void TestPIDControllersProgram::beginPhPID(PID& phPID, PHSensor& phSensor) {
    currentPID = &phPID;
    currentSensor = &phSensor;
    this->phSensor = &phSensor;
    running = true;
    startTime = millis();
    Serial.println("Running pH PID controller test...");
}

void TestPIDControllersProgram::beginDoPID(PID& doPID, OxygenSensor& oxygenSensor) {
    currentPID = &doPID;
    currentSensor = &oxygenSensor;
    this->oxygenSensor = &oxygenSensor;
    running = true;
    startTime = millis();
    Serial.println("Running Dissolved Oxygen PID controller test...");
}


void TestPIDControllersProgram::update() {
    if (!running) return;

    unsigned long elapsedTime = millis() - startTime;

    if (elapsedTime < 10000) {
        // Read the current sensor value
        if (currentSensor == &waterTempSensor) {
            tempInput = waterTempSensor.readValue();
        } else if (currentSensor == &phSensor) {
            phInput = phSensor.readValue();
        } else if (currentSensor == &oxygenSensor) {
            doInput = oxygenSensor.readValue();
        }

        // Compute the PID output
        currentPID->Compute();

        // Print the PID output
        Serial.print("PID output: ");
        Serial.println(currentPID->GetOutput());
    } else {
        running = false;
        Serial.println("PID controller test completed.");
    }
}

bool TestPIDControllersProgram::isRunning() {
    return running;
}