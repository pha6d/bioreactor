#include "TestActuatorsProgram.h"

TestActuatorsProgram::TestActuatorsProgram(Logger& logger, PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                                           PHSensor& phSensor, TurbiditySensor& turbiditySensor,
                                           OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor)
    : logger(logger), waterTempSensor(waterTempSensor), airTempSensor(airTempSensor),
      phSensor(phSensor), turbiditySensor(turbiditySensor),
      oxygenSensor(oxygenSensor), airFlowSensor(airFlowSensor),
      running(false), paused(false), startTime(0), lastPrintTime(0),
      currentTest(0), currentIndividualTest(""), individualTestValue(0),
      individualTestDuration(0), isSensorTestRunning(false) {}

void TestActuatorsProgram::configure(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor,
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


    logger.logInfo("TestActuatorsProgram configured successfully");
}


void TestActuatorsProgram::begin() {
    running = true;
    paused = false;
    startTime = millis();
    currentTest = 0;
    currentIndividualTest = "";
    isSensorTestRunning = false;
    logger.logInfo("Starting TestActuatorsProgram");
}

void TestActuatorsProgram::update() {
    if (!running || paused) return;

    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - startTime;

    if (isSensorTestRunning) {
        if (currentTime - lastPrintTime >= SENSOR_PRINT_INTERVAL) {
            lastPrintTime = currentTime;
            printSensorData();
        }
    } else if (currentIndividualTest.length() > 0) {
        updateIndividualTest(elapsedTime);
    } else {
        runFullTestSequence(elapsedTime);
    }
}

void TestActuatorsProgram::pause() {
    if (running && !paused) {
        paused = true;
        logger.logInfo("TestActuatorsProgram paused");
    }
}

void TestActuatorsProgram::resume() {
    if (running && paused) {
        paused = false;
        logger.logInfo("TestActuatorsProgram resumed");
    }
}

void TestActuatorsProgram::stop() {
    if (!running) return;

    running = false;
    paused = false;
    isSensorTestRunning = false;
    currentIndividualTest = "";
    currentTest = 0;

    // Arrêter tous les actionneurs
    if (airPump) airPump->control(false, 0);
    if (drainPump) drainPump->control(false, 0);
    if (stirringMotor) stirringMotor->control(false, 0);
    if (nutrientPump) nutrientPump->control(false, 0);
    if (basePump) basePump->control(false, 0);
    if (heatingPlate) heatingPlate->control(false);
    if (ledGrowLight) ledGrowLight->control(false);

    logger.logInfo("TestActuatorsProgram stopped");
}

bool TestActuatorsProgram::isRunning() const {
    return running;
}

void TestActuatorsProgram::runIndividualTest(const String& actuatorName, float value, int duration) {
    currentIndividualTest = actuatorName;
    individualTestValue = value;
    individualTestDuration = duration;
    startTime = millis();
    running = true;
    isSensorTestRunning = false;
    logger.logInfo("Starting individual test: " + actuatorName);
}

void TestActuatorsProgram::startSensorTest() {
    running = true;
    isSensorTestRunning = true;
    lastPrintTime = 0;
    startTime = millis();
    logger.logInfo("Starting sensors test");
}

void TestActuatorsProgram::printSensorData() {
    logger.logInfo("Sensor Data:");
    logger.logInfo("Water Temperature: " + String(waterTempSensor.readValue()) + " °C");
    logger.logInfo("Air Temperature: " + String(airTempSensor.readValue()) + " °C");
    logger.logInfo("pH Value: " + String(phSensor.readValue()));
    logger.logInfo("Turbidity: " + String(turbiditySensor.readValue()));
    logger.logInfo("Dissolved Oxygen: " + String(oxygenSensor.readValue()) + " mg/L");
    
    float flowRate = airFlowSensor.readValue();
    if (flowRate >= 0) {
        logger.logInfo("Air Flow Rate: " + String(flowRate) + " L/min");
    } else {
        logger.logWarning("Air Flow Sensor reading error");
    }
}


void TestActuatorsProgram::updateIndividualTest(unsigned long elapsedTime) {
    if (elapsedTime < static_cast<unsigned long>(individualTestDuration) * 1000) {
        if (currentIndividualTest == "airPump") airPump->control(true, individualTestValue);
        else if (currentIndividualTest == "drainPump") drainPump->control(true, individualTestValue);
        else if (currentIndividualTest == "stirringMotor") stirringMotor->control(true, individualTestValue);
        else if (currentIndividualTest == "nutrientPump") nutrientPump->control(true, individualTestValue);
        else if (currentIndividualTest == "basePump") basePump->control(true, individualTestValue);
        else if (currentIndividualTest == "heatingPlate") heatingPlate->control(true, individualTestValue);
        else if (currentIndividualTest == "ledGrowLight") ledGrowLight->control(true, individualTestValue);
    } else {
        stop();
    }
}

void TestActuatorsProgram::runFullTestSequence(unsigned long elapsedTime) {
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
            stop();
            break;
    }
}

