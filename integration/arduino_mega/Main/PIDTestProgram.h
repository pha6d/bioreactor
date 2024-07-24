#ifndef PID_TEST_PROGRAM_H
#define PID_TEST_PROGRAM_H

#include "ProgramBase.h"
#include "PIDManager.h"
#include "PT100Sensor.h"
#include "PHSensor.h"
#include "OxygenSensor.h"
#include "HeatingPlate.h"
#include "DCPump.h"

class PIDTestProgram : public ProgramBase {
public:
    PIDTestProgram(PIDManager& pidManager);
    void begin() override;
    void configure(const String& pidType, double setpoint);
    void update() override;
    void pause() override;
    void resume() override;
    void stop() override;
    bool isRunning() const override;
    String getName() const override { return "PIDTest"; }

    void setWaterTempSensor(PT100Sensor* sensor) { waterTempSensor = sensor; }
    void setPhSensor(PHSensor* sensor) { phSensor = sensor; }
    void setOxygenSensor(OxygenSensor* sensor) { oxygenSensor = sensor; }
    void setHeatingPlate(HeatingPlate* plate) { heatingPlate = plate; }
    void setAirPump(DCPump* pump) { airPump = pump; }

private:
    PIDManager& pidManager;
    PT100Sensor* waterTempSensor;
    PHSensor* phSensor;
    OxygenSensor* oxygenSensor;
    HeatingPlate* heatingPlate;
    DCPump* airPump;

    String currentPIDType;
    bool running;
    bool paused;
    unsigned long startTime;

    static const unsigned long TEST_DURATION = 300000; // 5 minutes
};

#endif // PID_TEST_PROGRAM_H