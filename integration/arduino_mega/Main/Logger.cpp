// Logger.cpp
#include "Logger.h"
#include <ArduinoJson.h>

LogLevel Logger::currentLevel = LogLevel::INFO;

void Logger::log(LogLevel level, const String& message) {
    if (level >= currentLevel) {
        String prefix;
        switch (level) {
            case LogLevel::DEBUG: prefix = "DEBUG: "; break;
            case LogLevel::INFO: prefix = "INFO: "; break;
            case LogLevel::WARNING: prefix = "WARNING: "; break;
            case LogLevel::ERROR: prefix = "ERROR: "; break;
        }
        Serial.println(prefix + message);
    }
}

#include "Logger.h"
#include "SensorController.h"
#include "ActuatorController.h"

void Logger::logData(const String& currentProgram, const String& programStatus) {
    // Read sensor values
    float wTemp = SensorController::readSensor("waterTempSensor");
    float aTemp = SensorController::readSensor("airTempSensor");
    float pH = SensorController::readSensor("phSensor");
    float turb = SensorController::readSensor("turbiditySensor");
    float oxy = SensorController::readSensor("oxygenSensor");
    float aflow = SensorController::readSensor("airFlowSensor");

    // Read actuator statuses
    bool apStat = ActuatorController::isActuatorRunning("airPump");
    bool dpStat = ActuatorController::isActuatorRunning("drainPump");
    bool npStat = ActuatorController::isActuatorRunning("nutrientPump");
    bool bpStat = ActuatorController::isActuatorRunning("basePump");
    bool smStat = ActuatorController::isActuatorRunning("stirringMotor");
    bool hpStat = ActuatorController::isActuatorRunning("heatingPlate");
    bool lgStat = ActuatorController::isActuatorRunning("ledGrowLight");

    // Create JSON document
    StaticJsonDocument<256> doc;

    // Populate JSON document with sensor data and statuses
    doc["program"] = currentProgram;
    doc["status"] = programStatus;
    doc["airP"] = apStat ? 1 : 0;
    doc["drainP"] = dpStat ? 1 : 0;
    doc["nutrientP"] = npStat ? 1 : 0;
    doc["baseP"] = bpStat ? 1 : 0;
    doc["stirringM"] = smStat ? 1 : 0;
    doc["heatingP"] = hpStat ? 1 : 0;
    doc["led"] = lgStat ? 1 : 0;
    doc["waterTemp"] = wTemp;
    doc["airTemp"] = aTemp;
    doc["pH"] = pH;
    doc["turbidity"] = turb;
    doc["oxygen"] = oxy;
    doc["airFlow"] = aflow;

    // Serialize JSON to string
    String output;
    serializeJson(doc, output);

    // Print JSON string to serial
    Serial.println(output);
}

void Logger::logStartupParameters(const String& programType, int rateOrSpeed, int duration,
    float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
    float baseConc, const String& experimentName, const String& comment) {
    
    StaticJsonDocument<256> doc;

    doc["ev"] = "startup";
    doc["pt"] = programType;
    doc["rate"] = rateOrSpeed;
    doc["dur"] = duration;
    doc["tSet"] = tempSetpoint;
    doc["phSet"] = phSetpoint;
    doc["doSet"] = doSetpoint;
    doc["nutC"] = nutrientConc;
    doc["baseC"] = baseConc;
    doc["expN"] = experimentName;
    doc["comm"] = comment;

    String output;
    serializeJson(doc, output);
    Serial.println(output);
}

void Logger::logPIDData(const String& pidType, float setpoint, float input, float output) {
    StaticJsonDocument<128> doc;
    
    doc["ev"] = "pid";
    doc["type"] = pidType;
    doc["set"] = setpoint;
    doc["in"] = input;
    doc["out"] = output;
    
    String jsonOutput;
    serializeJson(doc, jsonOutput);
    Serial.println(jsonOutput);
}

void Logger::setLogLevel(LogLevel level) {
    currentLevel = level;
}

void Logger::logSensorData() {
    log(LogLevel::INFO, "Water Temperature: " + String(SensorController::readSensor("waterTempSensor")) + " °C");
    log(LogLevel::INFO, "Air Temperature: " + String(SensorController::readSensor("airTempSensor")) + " °C");
    log(LogLevel::INFO, "pH: " + String(SensorController::readSensor("phSensor")));
    log(LogLevel::INFO, "Turbidity: " + String(SensorController::readSensor("turbiditySensor")) + " voltage");
    log(LogLevel::INFO, "Dissolved Oxygen: " + String(SensorController::readSensor("oxygenSensor")) + " mg/L");
    log(LogLevel::INFO, "Air Flow: " + String(SensorController::readSensor("airFlowSensor")) + " L/min");
}

void Logger::logActuatorData() {
    log(LogLevel::INFO, "Air Pump: " + String(ActuatorController::isActuatorRunning("Air Pump") ? "ON" : "OFF"));
    log(LogLevel::INFO, "Drain Pump: " + String(ActuatorController::isActuatorRunning("Drain Pump") ? "ON" : "OFF"));
    log(LogLevel::INFO, "Nutrient Pump: " + String(ActuatorController::isActuatorRunning("Nutrient Pump") ? "ON" : "OFF"));
    log(LogLevel::INFO, "Base Pump: " + String(ActuatorController::isActuatorRunning("Base Pump") ? "ON" : "OFF"));
    log(LogLevel::INFO, "Stirring Motor: " + String(ActuatorController::isActuatorRunning("Stirring Motor") ? "ON" : "OFF"));
    log(LogLevel::INFO, "Heating Plate: " + String(ActuatorController::isActuatorRunning("Heating Plate") ? "ON" : "OFF"));
    log(LogLevel::INFO, "LED Grow Light: " + String(ActuatorController::isActuatorRunning("LED Grow Light") ? "ON" : "OFF"));
}

