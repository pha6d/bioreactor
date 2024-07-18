#ifndef LOGGER_H
#define LOGGER_H

#include "DCPump.h"
#include "PeristalticPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"
#include "PT100Sensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PHSensor.h"
#include "TurbiditySensor.h"
#include "OxygenSensor.h"
#include "AirFlowSensor.h"
#include <SoftwareSerial.h>
#include "SafetySystem.h"

// Forward declaration
enum class AlertLevel;

extern HardwareSerial& espSerial;

class Logger {
public:
    static void logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump,
        StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
        PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor,
        TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
        const String& currentProgram, const String& programStatus);

    static void logStartupParameters(const String& programType, int rateOrSpeed, int duration,
        float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
        float baseConc, const String& experimentName, const String& comment);

    static void logAlert(const String& message, AlertLevel level);

    // Nouvelles méthodes (ne seront pas envoyées à l'ESP pour le moment)
    static void logInfo(const String& message);
    static void logWarning(const String& message);
    static void logError(const String& message);

private:
    static bool isValidData(float wTemp, float aTemp, float pH, float turb, float oxy, float aflow);
    static bool areStatusesValid(int apStat, int dpStat, int npStat, int bpStat, int smStat, int hpStat, int lgStat);
};

#endif