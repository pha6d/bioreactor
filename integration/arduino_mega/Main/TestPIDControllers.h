#ifndef TEST_PID_CONTROLLERS_H
#define TEST_PID_CONTROLLERS_H

#include "PIDControllers.h"
#include "SensorInterface.h"
#include "PT100Sensor.h"
#include "PHSensor.h"
#include "OxygenSensor.h"


class TestPIDControllersProgram {

public:
    void beginTempPID(PID& tempPID, PT100Sensor& waterTempSensor);
    void beginPhPID(PID& phPID, PHSensor& phSensor);
    void beginDoPID(PID& doPID, OxygenSensor& oxygenSensor);
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