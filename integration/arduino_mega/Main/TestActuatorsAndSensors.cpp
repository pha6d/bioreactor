#include "TestActuatorsAndSensors.h"

void TestActuatorsAndSensorsProgram::begin(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor, PeristalticPump& nutrientPump, PeristalticPump& basePump, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight, PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor) {
    this->airPump = &airPump;
    this->drainPump = &drainPump;
    this->stirringMotor = &stirringMotor;
    this->nutrientPump = &nutrientPump;
    this->basePump = &basePump;
    this->heatingPlate = &heatingPlate;
    this->ledGrowLight = &ledGrowLight;
    this->waterTempSensor = &waterTempSensor;
    this->airTempSensor = &airTempSensor;
    this->phSensor = &phSensor;
    this->turbiditySensor = &turbiditySensor;
    this->oxygenSensor = &oxygenSensor;
    this->airFlowSensor = &airFlowSensor;

    this->running = true;
    this->startTime = millis();

    Serial.println(), Serial.println("run TESTS program");
}

void TestActuatorsAndSensorsProgram::update() {
    if (!running) return;

    unsigned long elapsedTime = millis() - startTime;

    if (elapsedTime < 5000) {
        airPump->control(true, 50);
    }
    else if (elapsedTime < 7000) {
        airPump->control(false, 0);
    }
    else if (elapsedTime < 12000) {
        drainPump->control(true, 80);
    }
    else if (elapsedTime < 14000) {
        drainPump->control(false, 0);
    }
    else if (elapsedTime < 19000) {
        stirringMotor->control(true, 1500);
    }
    else if (elapsedTime < 21000) {
        stirringMotor->control(false, 0);
    }
    else if (elapsedTime < 26000) {
        nutrientPump->control(true, 50);
    }
    else if (elapsedTime < 28000) {
        nutrientPump->control(false, 0);
    }
    else if (elapsedTime < 33000) {
        basePump->control(true, 30);
    }
    else if (elapsedTime < 35000) {
        basePump->control(false, 0);
    }
    else if (elapsedTime < 42000) {
        heatingPlate->control(true);
    }
    else if (elapsedTime < 44000) {
        heatingPlate->control(false);
    }
    else if (elapsedTime < 49000) {
        ledGrowLight->control(true);
    }
    else if (elapsedTime < 51000) {
        ledGrowLight->control(false);
    }
    else {
        running = false;
        Serial.println("Tests completed.");
    }

    // Read and print sensor values
    if (elapsedTime % 1000 == 0) {
        float waterTemperature = waterTempSensor->readValue();
        Serial.print("Water Temperature: ");
        Serial.print(waterTemperature);
        Serial.println(" °C");

        float airTemperature = airTempSensor->readValue();
        Serial.print("Air Temperature: ");
        Serial.print(airTemperature);
        Serial.println(" °C");

        float pHValue = phSensor->readValue();
        Serial.print("pH Value: ");
        Serial.println(pHValue);

        float turbidityValue = turbiditySensor->readValue();
        Serial.print("Turbidity Voltage: ");
        Serial.println(turbidityValue);

        float DOValue = oxygenSensor->readValue();
        Serial.print("Dissolved Oxygen: ");
        Serial.print(DOValue);
        Serial.println(" mg/L");

        float flowRate = airFlowSensor->readValue();
        if (flowRate != -1.0) {
            Serial.print("Air Flow Rate: ");
            Serial.print(flowRate);
            Serial.println(" L/min");
        }
    }
}

bool TestActuatorsAndSensorsProgram::isRunning() {
    return running;
}
