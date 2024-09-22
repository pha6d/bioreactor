// Logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void log(LogLevel level, const String& message);
    static void Logger::logData(const String& currentProgram, 
                     const String& programStatus,
                     float wTemp, float aTemp, float eTemp, float pH, float turb, float oxy, float aflow,
                     bool apStat, bool dpStat, bool spStat, bool npStat, bool bpStat, bool smStat, bool hpStat, bool lgStat);
    // static void logData(const String& currentProgram, const String& programStatus);
    static void logStartupParameters(const String& programType, int rateOrSpeed, int duration,
        float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
        float baseConc, const String& experimentName, const String& comment);
    static void logPIDData(const String& pidType, float setpoint, float input, float output);
    static void setLogLevel(LogLevel level);
    // static void logSensorData();
    // static void logActuatorData();

private:
    static LogLevel currentLevel;
};

#endif