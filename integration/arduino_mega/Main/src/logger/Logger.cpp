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

static void Logger::logData(const String& currentProgram, 
                     const String& programStatus,
                     float wTemp, float aTemp, float eTemp, float pH, float turb, float oxy, float aflow,
                     bool apStat, bool dpStat, bool spStat, bool npStat, bool bpStat, bool smStat, bool hpStat, bool lgStat) {

    // Debug logs for sensor values
    log(LogLevel::DEBUG, "Water Temp: " + String(wTemp));
    log(LogLevel::DEBUG, "Air Temp: " + String(aTemp));
    log(LogLevel::DEBUG, "Electronic Temp: " + String(eTemp));
    log(LogLevel::DEBUG, "pH: " + String(pH));
    log(LogLevel::DEBUG, "Turbidity: " + String(turb));
    log(LogLevel::DEBUG, "Oxygen: " + String(oxy));
    log(LogLevel::DEBUG, "Air Flow: " + String(aflow));

    // Create a JSON document with a fixed size
    StaticJsonDocument<256> doc;

    // Add program info to JSON
    doc["program"] = currentProgram;
    doc["status"] = programStatus;

    // Add actuator statuses to JSON
    doc["airP"] = apStat ? 1 : 0;
    doc["drainP"] = dpStat ? 1 : 0;
    doc["sampleP"] = spStat ? 1 : 0;
    doc["nutrientP"] = npStat ? 1 : 0;
    doc["baseP"] = bpStat ? 1 : 0;
    doc["stirringM"] = smStat ? 1 : 0;
    doc["heatingP"] = hpStat ? 1 : 0;
    doc["led"] = lgStat ? 1 : 0;

    // Add sensor values to JSON
    doc["waterTemp"] = wTemp;
    doc["airTemp"] = aTemp;
    doc["elecTemp"] = eTemp;
    doc["pH"] = pH;
    doc["turbidity"] = turb;
    doc["oxygen"] = oxy;
    doc["airFlow"] = aflow;

    // Serialize JSON document to string
    String output;
    serializeJson(doc, output);

    // Print JSON string to serial
    Serial.println(output);
}

/*
void Logger::logData(const String& currentProgram, const String& programStatus) {
    // Read sensor values
    float wTemp = SensorController::readSensor("waterTempSensor");
    float aTemp = SensorController::readSensor("airTempSensor");
    float eTemp = SensorController::readSensor("electronicTempSensor");
    float pH = SensorController::readSensor("phSensor");
    float turb = SensorController::readSensor("turbiditySensorSEN0554");
    float oxy = SensorController::readSensor("oxygenSensor");
    float aflow = SensorController::readSensor("airFlowSensor");

    // Read actuator statuses
    bool apStat = ActuatorController::isActuatorRunning("airPump");
    bool dpStat = ActuatorController::isActuatorRunning("drainPump");
    bool spStat = ActuatorController::isActuatorRunning("samplePump");
    bool npStat = ActuatorController::isActuatorRunning("nutrientPump");
    bool bpStat = ActuatorController::isActuatorRunning("basePump");
    bool smStat = ActuatorController::isActuatorRunning("stirringMotor");
    bool hpStat = ActuatorController::isActuatorRunning("heatingPlate");
    bool lgStat = ActuatorController::isActuatorRunning("ledGrowLight");

    // Debug logs
    log(LogLevel::DEBUG, "Water Temp: " + String(wTemp));
    log(LogLevel::DEBUG, "Air Temp: " + String(aTemp));
    log(LogLevel::DEBUG, "Electronic Temp: " + String(eTemp));
    log(LogLevel::DEBUG, "pH: " + String(pH));
    log(LogLevel::DEBUG, "Turbidity: " + String(turb));
    log(LogLevel::DEBUG, "Oxygen: " + String(oxy));
    log(LogLevel::DEBUG, "Air Flow: " + String(aflow));

    // Construct JSON string manually
    String output = "{";
    output += "\"program\":\"" + currentProgram + "\",";
    output += "\"status\":\"" + programStatus + "\",";
    output += "\"airP\":" + String(apStat ? 1 : 0) + ",";
    output += "\"drainP\":" + String(dpStat ? 1 : 0) + ",";
    output += "\"sampleP\":" + String(spStat ? 1 : 0) + ",";
    output += "\"nutrientP\":" + String(npStat ? 1 : 0) + ",";
    output += "\"baseP\":" + String(bpStat ? 1 : 0) + ",";
    output += "\"stirringM\":" + String(smStat ? 1 : 0) + ",";
    output += "\"heatingP\":" + String(hpStat ? 1 : 0) + ",";
    output += "\"led\":" + String(lgStat ? 1 : 0) + ",";
    output += "\"waterTemp\":" + String(wTemp) + ",";
    output += "\"airTemp\":" + String(aTemp) + ",";
    output += "\"elecTemp\":" + String(eTemp) + ",";
    output += "\"pH\":" + String(pH) + ",";
    output += "\"turbidity\":" + String(turb) + ",";
    output += "\"oxygen\":" + String(oxy) + ",";
    output += "\"airFlow\":" + String(aflow);
    output += "}";

    // Print JSON string to serial
    Serial.println(output);
}
*/

// void Logger::logProgramEvent
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
