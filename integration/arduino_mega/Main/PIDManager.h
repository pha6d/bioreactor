#ifndef PID_MANAGER_H
#define PID_MANAGER_H

#include <PID_v1.h>
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "PeristalticPump.h"
#include "DCPump.h"
#include "PT100Sensor.h"
#include "PHSensor.h"
#include "OxygenSensor.h"

class PIDManager {
public:
    PIDManager(StirringMotor* stirringMotor, HeatingPlate* heatingPlate);
    
    void initialize(double tempKp, double tempKi, double tempKd,
                    double phKp, double phKi, double phKd,
                    double doKp, double doKi, double doKd);

    void updateAll(double tempInput, double phInput, double doInput);

    void setTemperatureSetpoint(double setpoint);
    void setPHSetpoint(double setpoint);
    void setDOSetpoint(double setpoint);

    double getTemperatureOutput() const;
    double getPHOutput() const;
    double getDOOutput() const;

    void startTemperaturePID(double setpoint);
    void startPHPID(double setpoint);
    void startDOPID(double setpoint);
    void updateTemperaturePID(double input);
    void updatePHPID(double input);
    void updateDOPID(double input);
    void stopTemperaturePID();
    void stopPHPID();
    void stopDOPID();
    void pauseAllPID();
    void resumeAllPID();

    void adjustStirringSpeed(int minSpeed);

    void saveParameters(const char* filename);
    void loadParameters(const char* filename);

    static const unsigned long UPDATE_INTERVAL_TEMP = 1000; // 1 second for temperature
    static const unsigned long UPDATE_INTERVAL_PH = 5000;   // 5 seconds for pH
    static const unsigned long UPDATE_INTERVAL_DO = 10000;  // 10 seconds for dissolved oxygen

    void configure(const String& pidType, double setpoint);

private:
    PID tempPID;
    PID phPID;
    PID doPID;

    double tempInput, tempOutput, tempSetpoint;
    double phInput, phOutput, phSetpoint;
    double doInput, doOutput, doSetpoint;

    StirringMotor* stirringMotor;
    HeatingPlate* heatingPlate;

    bool tempPIDRunning;
    bool phPIDRunning;
    bool doPIDRunning;

    unsigned long lastTempUpdateTime;
    unsigned long lastPHUpdateTime;
    unsigned long lastDOUpdateTime;

    void logPIDValues(const String& type, double setpoint, double input, double output);
};

#endif // PID_MANAGER_H