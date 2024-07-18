#include "Logger.h"
#include "SafetySystem.h"
#include <ArduinoJson.h>

HardwareSerial& espSerial = Serial1;


bool Logger::isValidData(float wTemp, float aTemp, float pH, float turb, float oxy, float aflow) {
    return true; // Simplification, toujours retourne vrai
}

bool Logger::areStatusesValid(int apStat, int dpStat, int npStat, int bpStat, int smStat, int hpStat, int lgStat) {
    return true; // Simplification, toujours retourne vrai
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
        DynamicJsonDocument doc(2048);

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
        doc["oxyden"] = oxy;
        doc["airFlow"] = aflow;

        String data;
        serializeJson(doc, data);
        data += "\n"; // Ajout d'un caractère de fin de ligne

        espSerial.println(data); // Envoi des données au format JSON via Serial1
        Serial.println(data); // Affichage des données pour débogage
    }
    else {
        Serial.println("Invalid sensor data or status detected.");
    }
}

void Logger::logStartupParameters(const String& programType, int rateOrSpeed, int duration,
    float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
    float baseConc, const String& experimentName, const String& comment) {

    DynamicJsonDocument doc(2048);

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
    data += "\n"; // Ajout d'un caractère de fin de ligne

    espSerial.println(data); // Envoi des données au format JSON via Serial1
    Serial.println(data); // Affichage des données pour débogage
}

void Logger::logAlert(const String& message, AlertLevel level) {
    String levelStr = (level == AlertLevel::WARNING) ? "WARNING" : "ALARM";
    String alertMessage = levelStr + ": " + message;
    Serial.println(alertMessage);
    // Vous pouvez ajouter ici l'envoi de l'alerte à l'ESP32 si nécessaire
}

// Nouvelles méthodes (ne seront pas envoyées à l'ESP pour le moment)
void Logger::logInfo(const String& message) {
    Serial.println("INFO: " + message);
}

void Logger::logWarning(const String& message) {
    Serial.println("WARNING: " + message);
}

void Logger::logError(const String& message) {
    Serial.println("ERROR: " + message);
}