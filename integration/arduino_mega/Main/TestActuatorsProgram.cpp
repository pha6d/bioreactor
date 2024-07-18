#include "TestActuatorsProgram.h"
#include <Arduino.h>

void TestActuatorsProgram::begin(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor,
                                 PeristalticPump& nutrientPump, PeristalticPump& basePump,
                                 HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                                 PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                                 PHSensor& phSensor, TurbiditySensor& turbiditySensor,
                                 OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor) {
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
    this->lastPrintTime = 0;
    this->currentTest = 0;

    Serial.println("Starting TESTS program");
}

void TestActuatorsProgram::update() {
    if (!running) return;

    unsigned long elapsedTime = millis() - startTime;
    unsigned long currentTime = millis();

    if (currentTime - lastPrintTime >= 1000) {
        lastPrintTime = currentTime;
        printSensorData();
    }

    switch (currentTest) {
        case 0:
            if (elapsedTime < 5000) {
                airPump->control(true, 50);
            } else {
                airPump->control(false, 0);
                currentTest++;
                Serial.println("Air Pump test completed");
            }
            break;
        case 1:
            if (elapsedTime < 10000) {
                drainPump->control(true, 80);
            } else {
                drainPump->control(false, 0);
                currentTest++;
                Serial.println("Drain Pump test completed");
            }
            break;
        case 2:
            if (elapsedTime < 15000) {
                stirringMotor->control(true, 1500);
            } else {
                stirringMotor->control(false, 0);
                currentTest++;
                Serial.println("Stirring Motor test completed");
            }
            break;
        case 3:
            if (elapsedTime < 20000) {
                nutrientPump->control(true, 50);
            } else {
                nutrientPump->control(false, 0);
                currentTest++;
                Serial.println("Nutrient Pump test completed");
            }
            break;
        case 4:
            if (elapsedTime < 25000) {
                basePump->control(true, 30);
            } else {
                basePump->control(false, 0);
                currentTest++;
                Serial.println("Base Pump test completed");
            }
            break;
        case 5:
            if (elapsedTime < 30000) {
                heatingPlate->control(true);
            } else {
                heatingPlate->control(false);
                currentTest++;
                Serial.println("Heating Plate test completed");
            }
            break;
        case 6:
            if (elapsedTime < 35000) {
                ledGrowLight->control(true);
            } else {
                ledGrowLight->control(false);
                currentTest++;
                Serial.println("LED Grow Light test completed");
            }
            break;
        default:
            running = false;
            Serial.println("All tests completed");
            break;
    }
}

bool TestActuatorsProgram::isRunning() const {
    return running;
}

void TestActuatorsProgram::printSensorData() {
    Serial.print("Water Temperature: ");
    Serial.print(waterTempSensor->readValue());
    Serial.println(" °C");

    Serial.print("Air Temperature: ");
    Serial.print(airTempSensor->readValue());
    Serial.println(" °C");

    Serial.print("pH Value: ");
    Serial.println(phSensor->readValue());

    Serial.print("Turbidity Voltage: ");
    Serial.println(turbiditySensor->readValue());

    Serial.print("Dissolved Oxygen: ");
    Serial.print(oxygenSensor->readValue());
    Serial.println(" mg/L");

    float flowRate = airFlowSensor->readValue();
    if (flowRate != -1.0) {
        Serial.print("Air Flow Rate: ");
        Serial.print(flowRate);
        Serial.println(" L/min");
    }

    Serial.println(); // Add a blank line for readability
}