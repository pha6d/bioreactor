#include "SensorController.h"

// Static pointers, initialized to nullptr
PT100Sensor* SensorController::waterTempSensor = nullptr;
DS18B20TemperatureSensor* SensorController::airTempSensor = nullptr;
DS18B20TemperatureSensor* SensorController::electronicTempSensor = nullptr;
PHSensor* SensorController::phSensor = nullptr;
OxygenSensor* SensorController::oxygenSensor = nullptr;
AirFlowSensor* SensorController::airFlowSensor = nullptr;
TurbiditySensorSEN0554* SensorController::turbiditySensorSEN0554 = nullptr;

// Initialize method
void SensorController::initialize(PT100Sensor& waterTemp, DS18B20TemperatureSensor& airTemp, DS18B20TemperatureSensor& electronicTemp,
                                  PHSensor& ph,
                                  OxygenSensor& oxygen, 
                                  AirFlowSensor& airFlow,
                                  TurbiditySensorSEN0554& turbiditySEN0554) {
    // Assign addresses of sensor objects to the static pointers
    waterTempSensor = &waterTemp;
    electronicTempSensor = &electronicTemp;
    airTempSensor = &airTemp;
    phSensor = &ph;
    oxygenSensor = &oxygen;
    airFlowSensor = &airFlow;
    turbiditySensorSEN0554 = &turbiditySEN0554;
}

void SensorController::beginAll() {
    waterTempSensor->begin();
    airTempSensor->begin();
    electronicTempSensor->begin();
    phSensor->begin();
    oxygenSensor->begin();
    airFlowSensor->begin();
    turbiditySensorSEN0554->begin();
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
    readSensor(electronicTempSensor->getName());
    readSensor(phSensor->getName());
    readSensor(oxygenSensor->getName());
    readSensor(airFlowSensor->getName());
    readSensor(turbiditySensorSEN0554->getName());
}

SensorInterface* SensorController::findSensorByName(const String& name) {
    if (name == waterTempSensor->getName()) return waterTempSensor;
    if (name == airTempSensor->getName()) return airTempSensor;
    if (name == electronicTempSensor->getName()) return electronicTempSensor;
    if (name == phSensor->getName()) return phSensor;
    if (name == oxygenSensor->getName()) return oxygenSensor;
    if (name == airFlowSensor->getName()) return airFlowSensor;
    if (name == turbiditySensorSEN0554->getName()) return turbiditySensorSEN0554;
    return nullptr;
}

static void SensorController::logSensorData() {
    Logger::log(LogLevel::INFO, "Water Temperature: " + String(SensorController::readSensor("waterTempSensor")) + " °C");
    Logger::log(LogLevel::INFO, "Air Temperature: " + String(SensorController::readSensor("airTempSensor")) + " °C");
    Logger::log(LogLevel::INFO, "Electronic Temperature: " + String(SensorController::readSensor("electronicTempSensor")) + " °C");
    Logger::log(LogLevel::INFO, "pH: " + String(SensorController::readSensor("phSensor")));
    Logger::log(LogLevel::INFO, "Turbidity: " + String(SensorController::readSensor("turbiditySensorSEN0554")) + " voltage");
    Logger::log(LogLevel::INFO, "Dissolved Oxygen: " + String(SensorController::readSensor("oxygenSensor")) + " mg/L");
    Logger::log(LogLevel::INFO, "Air Flow: " + String(SensorController::readSensor("airFlowSensor")) + " L/min");
}