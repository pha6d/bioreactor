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
    // Generic method for executing an actuator
    static void runActuator(const String& actuatorName, float value, int duration);

    // Method for executing an actuator from the console (for tests)
    static void executeActuator(const String& command);

private:
    static void runPeristalticPump(PeristalticPump& pump, float flowRate, int duration);
    static void runDCPump(DCPump& pump, int speed, int duration);
    static void runStirringMotor(StirringMotor& motor, int speed, int duration);
    static void runHeatingPlate(HeatingPlate& heatingPlate, int temperature, int duration);
    static void runLEDGrowLight(LEDGrowLight& light, int intensity, int duration);

    static void logActuatorOperation(const String& actuatorName, float value, int duration);
};

#endif
