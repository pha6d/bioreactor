#ifndef PID_MANAGER_H
#define PID_MANAGER_H

#include <PID_v1.h>
#include "StirringMotor.h"

class PIDManager {
public:
    PIDManager(StirringMotor& stirringMotor);
    
    // Initialize PID controllers with given parameters
    void initialize(double tempKp, double tempKi, double tempKd,
                    double phKp, double phKi, double phKd,
                    double doKp, double doKi, double doKd);
    
    // Update all PID controllers with new input values
    void updateAll(double tempInput, double phInput, double doInput);
    
    // Getters for PID outputs
    double getTemperatureOutput() const;
    double getPHOutput() const;
    double getDOOutput() const;
    
    // Setters for PID setpoints
    void setTemperatureSetpoint(double setpoint);
    void setPHSetpoint(double setpoint);
    void setDOSetpoint(double setpoint);
    
    // Methods for PID testing
    void startTest(const String& type);
    void updateTest();
    void stopTest();
    bool isTestRunning() const;
    
    // Method to adjust stirring speed based on PID outputs and minimum speed
    void adjustStirringSpeed(int minSpeed);
    
    // Method to start/stop the PID manager
    void start() { running = true; }
    void stop() { running = false; }

private:
    PID tempPID;
    PID phPID;
    PID doPID;
    double tempInput, tempOutput, tempSetpoint;
    double phInput, phOutput, phSetpoint;
    double doInput, doOutput, doSetpoint;
    StirringMotor& stirringMotor;

    bool testRunning;
    String testType;
    unsigned long lastTestUpdate;
    static const unsigned long TEST_INTERVAL = 1000; // 1 second

    bool running;  // Flag to indicate if the PID manager is running
    bool stopFlag; // Flag to stop the PID manager

    // Helper method to log PID values during testing
    void logPIDValues(const String& type, double setpoint, double input, double output);
};

#endif // PID_MANAGER_H