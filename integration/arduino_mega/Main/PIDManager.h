// PIDManager.h
#ifndef PID_MANAGER_H
#define PID_MANAGER_H

#include <PID_v1.h>
#include "ActuatorController.h"
#include "SensorController.h"
#include "VolumeManager.h"

class PIDManager {
public:
    PIDManager();
    
    void initialize(double tempKp, double tempKi, double tempKd,
                    double phKp, double phKi, double phKd,
                    double doKp, double doKi, double doKd);

    void updateAllPIDControllers();

    void setTemperatureSetpoint(double setpoint);
    void setPHSetpoint(double setpoint);
    void setDOSetpoint(double setpoint);

    void startTemperaturePID(double setpoint);
    void startPHPID(double setpoint);
    void startDOPID(double setpoint);
    void updateTemperaturePID();
    void updatePHPID();
    void updateDOPID();
    void stopTemperaturePID();
    void stopPHPID();
    void stopDOPID();
    void stopAllPID();
    void pauseAllPID();
    void resumeAllPID();

    double getTemperatureOutput() const;
    double getPHOutput() const;
    double getDOOutput() const;
    void adjustPIDStirringSpeed();

    void saveParameters(const char* filename);
    void loadParameters(const char* filename);

    void setMinStirringSpeed(int speed) { minStirringSpeed = speed; }
    int getMinStirringSpeed() const { return minStirringSpeed; }

    void adjustPIDParameters(const String& pidType, double Kp, double Ki, double Kd);
    //void saveParametersToEEPROM();
    //void loadParametersFromEEPROM();
private:
    PID tempPID;
    PID phPID;
    PID doPID;

    double tempInput, tempOutput, tempSetpoint;
    double phInput, phOutput, phSetpoint;
    double doInput, doOutput, doSetpoint;

    bool tempPIDRunning;
    bool phPIDRunning;
    bool doPIDRunning;

    unsigned long lastTempUpdateTime;
    unsigned long lastPHUpdateTime;
    unsigned long lastDOUpdateTime;

    static const unsigned long UPDATE_INTERVAL_TEMP = 10000; // 20 seconds - (10-20 seconds; usually in the chemical process industry ) ; could be appropriate if the changes are rapid: 1 second
    static const unsigned long UPDATE_INTERVAL_PH = 30000;   // 45 seconds - (30-60 seconds; usually in the chemical process industry ) ; could be appropriate if the changes are rapid: 5 seconds
    static const unsigned long UPDATE_INTERVAL_DO = 30000;  // 45 seconds - (30-60 seconds; usually in the chemical process industry ) ; could be appropriate if the changes are rapid: 10 seconds

    int minStirringSpeed;
};

#endif // PID_MANAGER_H