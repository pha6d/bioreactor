#include "PIDControllers.h"

// Define PID variables
double tempSetpoint, tempInput, tempOutput, tempMyOutput;
double phSetpoint, phInput, phOutput, phMyOutput;
double doSetpoint, doInput, doOutput, doMyOutput;

// Initialize PID controllers
PID tempPID(&tempInput, &tempOutput, &tempSetpoint, 2.0, 5.0, 1.0, DIRECT);
PID phPID(&phInput, &phOutput, &phSetpoint, 2.0, 5.0, 1.0, DIRECT);
PID doPID(&doInput, &doOutput, &doSetpoint, 2.0, 5.0, 1.0, DIRECT);

void initializePIDs(double tempKp, double tempKi, double tempKd,
                    double phKp, double phKi, double phKd,
                    double doKp, double doKi, double doKd) {
    tempPID.SetTunings(tempKp, tempKi, tempKd);
    tempPID.SetMode(AUTOMATIC);
    tempMyOutput = &tempOutput;

    phPID.SetTunings(phKp, phKi, phKd);
    phPID.SetMode(AUTOMATIC);
    phMyOutput = &phOutput;

    doPID.SetTunings(doKp, doKi, doKd);
    doPID.SetMode(AUTOMATIC);
    doMyOutput = &doOutput;
}