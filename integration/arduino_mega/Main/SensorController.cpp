#include "SensorController.h"
#include "Logger.h"

// Static pointers, initialized to nullptr
PT100Sensor* SensorController::waterTempSensor = nullptr;
DS18B20TemperatureSensor* SensorController::airTempSensor = nullptr;
PHSensor* SensorController::phSensor = nullptr;
TurbiditySensor* SensorController::turbiditySensor = nullptr;
OxygenSensor* SensorController::oxygenSensor = nullptr;
AirFlowSensor* SensorController::airFlowSensor = nullptr;

// Initialize method
void SensorController::initialize(PT100Sensor& waterTemp, DS18B20TemperatureSensor& airTemp,
                                  PHSensor& ph, TurbiditySensor& turbidity,
                                  OxygenSensor& oxygen, AirFlowSensor& airFlow) {
    // Assign addresses of sensor objects to the static pointers
    waterTempSensor = &waterTemp;
    airTempSensor = &airTemp;
    phSensor = &ph;
    turbiditySensor = &turbidity;
    oxygenSensor = &oxygen;
    airFlowSensor = &airFlow;
}

void SensorController::beginAll() {
    waterTempSensor->begin();
    airTempSensor->begin();
    phSensor->begin();
    turbiditySensor->begin();
    oxygenSensor->begin();
    airFlowSensor->begin();
}

float SensorController::readSensor(const String& sensorName) {
    SensorInterface* sensor = findSensorByName(sensorName);
    if (sensor) {
        float value = sensor->readValue();
        //Logger::log(LogLevel::INFO, "Read sensor " + sensorName + ": " + String(value));
        return value;
    }
    Logger::log(LogLevel::WARNING, "Sensor not found: " + sensorName);
    return 0.0f;
}

void SensorController::updateAllSensors() {
    readSensor(waterTempSensor->getName());
    readSensor(airTempSensor->getName());
    readSensor(phSensor->getName());
    readSensor(turbiditySensor->getName());
    readSensor(oxygenSensor->getName());
    readSensor(airFlowSensor->getName());
}

SensorInterface* SensorController::findSensorByName(const String& name) {
    if (name == waterTempSensor->getName()) return waterTempSensor;
    if (name == airTempSensor->getName()) return airTempSensor;
    if (name == phSensor->getName()) return phSensor;
    if (name == turbiditySensor->getName()) return turbiditySensor;
    if (name == oxygenSensor->getName()) return oxygenSensor;
    if (name == airFlowSensor->getName()) return airFlowSensor;
    return nullptr;
}