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

extern bool stopFlag;

class PIDManager {
public:
    // Constructor
    PIDManager(StirringMotor* stirringMotor, HeatingPlate* heatingPlate);
    
    // Initialize PID controllers
    void initialize(double tempKp, double tempKi, double tempKd,
                    double phKp, double phKi, double phKd,
                    double doKp, double doKi, double doKd);

    // Update all PID controllers
    void updateAll(double tempInput, double phInput, double doInput);

    // Set setpoints for each PID
    void setTemperatureSetpoint(double setpoint);
    void setPHSetpoint(double setpoint);
    void setDOSetpoint(double setpoint);

    // Get outputs from each PID
    double getTemperatureOutput() const;
    double getPHOutput() const;
    double getDOOutput() const;

    // Test-related methods
    void startTest(const String& type);
    void updateTest();
    void stopTest();
    bool isTestRunning() const;

    // Adjust stirring speed based on PID outputs
    void adjustStirringSpeed(int minSpeed);

    // Check if stop flag is set
    void checkStopFlag();

    // New methods for individual PID control
    void startTemperaturePID(double setpoint);
    void startPHPID(double setpoint);
    void startDOPID(double setpoint);
    void updateTemperaturePID(double input);
    void updatePHPID(double input);
    void updateDOPID(double input);
    void stopTemperaturePID();
    void stopPHPID();
    void stopDOPID();

private:
    // PID controllers
    PID tempPID;
    PID phPID;
    PID doPID;

    // PID variables
    double tempInput, tempOutput, tempSetpoint;
    double phInput, phOutput, phSetpoint;
    double doInput, doOutput, doSetpoint;

    // Actuators
    StirringMotor* stirringMotor;
    HeatingPlate* heatingPlate;

    // Test variables
    bool testRunning;
    String testType;
    unsigned long lastTestUpdate;
    static const unsigned long TEST_INTERVAL = 1000; // 1 second

    // New variables for individual PID control
    bool tempPIDRunning;
    bool phPIDRunning;
    bool doPIDRunning;

    // Helper method to log PID values
    void logPIDValues(const String& type, double setpoint, double input, double output);
};

#endif // PID_MANAGER_H