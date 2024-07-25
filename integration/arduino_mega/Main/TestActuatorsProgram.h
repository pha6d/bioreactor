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
#include "Logger.h"

class TestActuatorsProgram : public ProgramBase {
public:
    TestActuatorsProgram(Logger& logger, PT100Sensor& waterTempSensor,
                     DS18B20TemperatureSensor& airTempSensor,
                     PHSensor& phSensor, TurbiditySensor& turbiditySensor,
                     OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor);
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

    void runIndividualTest(const String& actuatorName, float value, int duration);
    void startSensorTest();

private:
    static const unsigned long SENSOR_PRINT_INTERVAL = 5000; // 5 secondes

    DCPump* airPump;
    DCPump* drainPump;
    StirringMotor* stirringMotor;
    PeristalticPump* nutrientPump;
    PeristalticPump* basePump;
    HeatingPlate* heatingPlate;
    LEDGrowLight* ledGrowLight;
    
    Logger& logger;
    PT100Sensor& waterTempSensor;
    DS18B20TemperatureSensor& airTempSensor;
    PHSensor& phSensor;
    TurbiditySensor& turbiditySensor;
    OxygenSensor& oxygenSensor;
    AirFlowSensor& airFlowSensor;

    bool running;
    bool paused;
    unsigned long startTime;
    unsigned long lastPrintTime;
    int currentTest;

    String currentIndividualTest;
    float individualTestValue;
    int individualTestDuration;

    bool isSensorTestRunning;

    void printSensorData();
    void runFullTestSequence(unsigned long elapsedTime);
    void updateIndividualTest(unsigned long elapsedTime);
};

#endif // TEST_ACTUATORS_PROGRAM_H