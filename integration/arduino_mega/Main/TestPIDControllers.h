#ifndef TEST_PID_CONTROLLERS_H
#define TEST_PID_CONTROLLERS_H

#include "PIDControllers.h"

class TestPIDControllersProgram {
public:
    void beginTempPID(PID& tempPID);
    void beginPhPID(PID& phPID);
    void beginDoPID(PID& doPID);
    void update();
    bool isRunning();

private:
    PID* currentPID;
    SensorInterface* currentSensor;
    PT100Sensor* waterTempSensor;
    PHSensor* phSensor;
    OxygenSensor* oxygenSensor;
    bool running;
    unsigned long startTime;
};

#endif