// PIDController.h
#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <Arduino.h>
#include <PID_v1.h>

class PIDController {
public:
    PIDController();
    void initialize(double tempKp, double tempKi, double tempKd,
                    double phKp, double phKi, double phKd,
                    double doKp, double doKi, double doKd);
    void updateAll(double tempInput, double phInput, double doInput);
    double getTemperatureOutput() const { return tempOutput; }
    double getPHOutput() const { return phOutput; }
    double getDOOutput() const { return doOutput; }

    void stopTest();
    
    // Méthodes de test
    void startTest(const String& type);
    void updateTest();
    bool isTestRunning() const { return testRunning; }

private:
    PID tempPID;
    PID phPID;
    PID doPID;
    double tempInput, tempOutput, tempSetpoint;
    double phInput, phOutput, phSetpoint;
    double doInput, doOutput, doSetpoint;

    // Variables pour le test
    bool testRunning;
    String testType;
    unsigned long lastTestUpdate;
    static const unsigned long TEST_INTERVAL = 5000; // 5 secondes
};

#endif // PID_CONTROLLER_H