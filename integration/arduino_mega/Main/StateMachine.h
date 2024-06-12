#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "DCPump.h"
#include "PeristalticPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"
#include "TestActuatorsAndSensors.h"
#include "Fermentation.h"
#include "Drain.h"
#include "Mix.h"

enum State {
    IDLE,
    RUNNING,
    STOPPING,
    COMPLETED
};

class StateMachine {
public:
    StateMachine();

    void startDrain(DCPump& drainPump, int rate, int duration);
    void startMix(StirringMotor& stirringMotor, int speed);
    void startTests(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor, PeristalticPump& nutrientPump,
        PeristalticPump& basePump, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
        PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
        PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor,
        AirFlowSensor& airFlowSensor);
    void startFermentation(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump,
        StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
        PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
        PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor,
        AirFlowSensor& airFlowSensor, float tempSetpoint, float phSetpoint,
        float doSetpoint, float nutrientConc, float baseConc, int duration,
        const String& experimentName, const String& comment);
    void stopAll(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
        PeristalticPump& basePump, StirringMotor& stirringMotor,
        HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);
    void update(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
        PeristalticPump& basePump, StirringMotor& stirringMotor,
        HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight);

    State getState();

private:
    State currentState;
    unsigned long startTime;

    DrainProgram drainProgram;
    MixProgram mixProgram;
    TestActuatorsAndSensorsProgram testProgram;
    FermentationProgram fermentationProgram;
};

#endif // STATE_MACHINE_H
