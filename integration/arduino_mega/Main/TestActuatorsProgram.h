#ifndef TEST_ACTUATORS_PROGRAM_H
#define TEST_ACTUATORS_PROGRAM_H

#include "ProgramBase.h"
#include "DCPump.h"
#include "PeristalticPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"
#include "PT100Sensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PHSensor.h"
#include "TurbiditySensor.h"
#include "OxygenSensor.h"
#include "AirFlowSensor.h"

class TestActuatorsProgram : public ProgramBase {
public:
    TestActuatorsProgram();
    void configure(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor,
               PeristalticPump& nutrientPump, PeristalticPump& basePump,
               HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
               PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
               PHSensor& phSensor, TurbiditySensor& turbiditySensor,
               OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor);
    void begin() override;
    void update() override;
    void pause() override;
    void resume() override;
    void stop() override;
    bool isRunning() const override;
    String getName() const override { return "TestActuators"; }

private:
    DCPump* airPump;
    DCPump* drainPump;
    StirringMotor* stirringMotor;
    PeristalticPump* nutrientPump;
    PeristalticPump* basePump;
    HeatingPlate* heatingPlate;
    LEDGrowLight* ledGrowLight;
    PT100Sensor* waterTempSensor;
    DS18B20TemperatureSensor* airTempSensor;
    PHSensor* phSensor;
    TurbiditySensor* turbiditySensor;
    OxygenSensor* oxygenSensor;
    AirFlowSensor* airFlowSensor;

    bool running;
    bool paused;
    unsigned long startTime;
    unsigned long lastPrintTime;
    int currentTest;

    void printSensorData();
};

#endif // TEST_ACTUATORS_PROGRAM_H