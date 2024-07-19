#include "Logger.h"
#include "SafetySystem.h"
#include <ArduinoJson.h>

HardwareSerial& espSerial = Serial1;

bool Logger::isValidData(float wTemp, float aTemp, float pH, float turb, float oxy, float aflow) {
    // Implement data validation logic here
    return true;
}

bool Logger::areStatusesValid(int apStat, int dpStat, int npStat, int bpStat, int smStat, int hpStat, int lgStat) {
    // Implement status validation logic here
    return true;
}

void Logger::logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump,
    StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor,
    TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
    const String& currentProgram, const String& programStatus) {

    float wTemp = waterTempSensor.readValue();
    float aTemp = airTempSensor.readValue();
    float pH = phSensor.readValue();
    float turb = turbiditySensor.readValue();
    float oxy = oxygenSensor.readValue();
    float aflow = airFlowSensor.readValue();

    int apStat = airPump.isOn() ? 1 : 0;
    int dpStat = drainPump.isOn() ? 1 : 0;
    int npStat = nutrientPump.isOn() ? 1 : 0;
    int bpStat = basePump.isOn() ? 1 : 0;
    int smStat = stirringMotor.isOn() ? 1 : 0;
    int hpStat = heatingPlate.isOn() ? 1 : 0;
    int lgStat = ledGrowLight.isOn() ? 1 : 0;

    if (isValidData(wTemp, aTemp, pH, turb, oxy, aflow) && areStatusesValid(apStat, dpStat, npStat, bpStat, smStat, hpStat, lgStat)) {
        StaticJsonDocument<2048> doc;

        doc["program"] = currentProgram;
        doc["status"] = programStatus;
        doc["airP"] = apStat;
        doc["drainP"] = dpStat;
        doc["nutrientP"] = npStat;
        doc["baseP"] = bpStat;
        doc["stirringM"] = smStat;
        doc["heatingP"] = hpStat;
        doc["led"] = lgStat;
        doc["waterTemp"] = wTemp;
        doc["airTemp"] = aTemp;
        doc["pH"] = pH;
        doc["turbidity"] = turb;
        doc["oxygen"] = oxy;
        doc["airFlow"] = aflow;

        String data;
        serializeJson(doc, data);
        data += "\n";

        espSerial.println(data);
        Serial.println(data);
    }
    else {
        Serial.println("Invalid sensor data or status detected.");
    }
}

void Logger::logStartupParameters(const String& programType, int rateOrSpeed, int duration,
    float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
    float baseConc, const String& experimentName, const String& comment) {

    StaticJsonDocument<2048> doc;

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

    String data;
    serializeJson(doc, data);
    data += "\n";

    espSerial.println(data);
    Serial.println(data);
}

void Logger::logAlert(const String& message, AlertLevel level) {
    String levelStr = (level == AlertLevel::WARNING) ? "WARNING" : "ALARM";
    String alertMessage = levelStr + ": " + message;
    Serial.println(alertMessage);
    // You can add here the logic to send the alert to the ESP32 if needed
}

void Logger::logInfo(const String& message) {
    Serial.println("INFO: " + message);
}

void Logger::logWarning(const String& message) {
    Serial.println("WARNING: " + message);
}

void Logger::logError(const String& message) {
    Serial.println("ERROR: " + message);
}

void Logger::logFermentationData(float waterTemp, float airTemp, float pH, float turbidity, float oxygenLevel, float airFlow,
                                 float tempSetpoint, float phSetpoint, float doSetpoint,
                                 float tempOutput, float phOutput, float doOutput) {
    StaticJsonDocument<2048> doc;

    doc["ev"] = "fermentation";
    doc["wTemp"] = waterTemp;
    doc["aTemp"] = airTemp;
    doc["pH"] = pH;
    doc["turb"] = turbidity;
    doc["oxy"] = oxygenLevel;
    doc["airFlow"] = airFlow;
    doc["tSet"] = tempSetpoint;
    doc["phSet"] = phSetpoint;
    doc["doSet"] = doSetpoint;
    doc["tOut"] = tempOutput;
    doc["phOut"] = phOutput;
    doc["doOut"] = doOutput;

    String data;
    serializeJson(doc, data);
    data += "\n";

    espSerial.println(data);
    Serial.println(data);
}

void Logger::logPIDData(const String& pidType, float setpoint, float input, float output) {
    StaticJsonDocument<2048> doc;

    doc["ev"] = "pid";
    doc["type"] = pidType;
    doc["set"] = setpoint;
    doc["in"] = input;
    doc["out"] = output;

    String data;
    serializeJson(doc, data);
    data += "\n";

    espSerial.println(data);
    Serial.println(data);
}