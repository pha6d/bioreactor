#ifndef ACTUATOR_CONTROLLER_H
#define ACTUATOR_CONTROLLER_H

#include "ActuatorInterface.h"
#include "PeristalticPump.h"
#include "DCPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"

class ActuatorController {
public:
    static void runActuator(const String& actuatorName, float value, int duration);
    static void executeActuator(const String& command);
    static void stopAllActuators();
    static bool isTestRunning();
    static void update();  

private:
    static void runPeristalticPump(PeristalticPump& pump, float flowRate, int duration);
    static void runDCPump(DCPump& pump, int speed, int duration);
    static void runStirringMotor(StirringMotor& motor, int speed, int duration);
    static void runHeatingPlate(HeatingPlate& heatingPlate, int temperature, int duration);
    static void runLEDGrowLight(LEDGrowLight& light, int intensity, int duration);
    static void logActuatorOperation(const String& actuatorName, float value, int duration);

    static bool testRunning;
    static String currentActuator;
    static unsigned long testStartTime;
    static int testDuration;
};

#endif