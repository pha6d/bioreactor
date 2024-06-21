#include "Logger.h"
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

HardwareSerial& espSerial = Serial1;

bool isValidData(float wTemp, float aTemp, float pH, float turb, float oxy, float aflow) {
    return true; // Simplification, toujours retourne vrai
}

bool areStatusesValid(int apStat, int dpStat, int npStat, int bpStat, int smStat, int hpStat, int lgStat) {
    return true; // Simplification, toujours retourne vrai
}

void logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump,
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

        doc["prog"] = currentProgram;
        doc["stat"] = programStatus;
        doc["ap"] = apStat;
        doc["dp"] = dpStat;
        doc["np"] = npStat;
        doc["bp"] = bpStat;
        doc["sm"] = smStat;
        doc["hp"] = hpStat;
        doc["lg"] = lgStat;
        doc["wT"] = wTemp;
        doc["aT"] = aTemp;
        doc["pH"] = pH;
        doc["tb"] = turb;
        doc["ox"] = oxy;
        doc["af"] = aflow;

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

bool isValidStartupParameters(const String& programType, int rateOrSpeed, int duration, float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc, float baseConc) {
    return programType.length() > 0 && rateOrSpeed >= 0 && duration >= 0 &&
        !isnan(tempSetpoint) && !isnan(phSetpoint) && !isnan(doSetpoint) &&
        !isnan(nutrientConc) && !isnan(baseConc);
}

void logStartupParameters(const String& programType, int rateOrSpeed, int duration,
    float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
    float baseConc, const String& experimentName, const String& comment) {

    if (isValidStartupParameters(programType, rateOrSpeed, duration, tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc)) {
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
    else {
        Serial.println("Invalid startup parameters detected.");
    }
}
