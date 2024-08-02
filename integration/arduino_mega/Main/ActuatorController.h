// ActuatorController.h
#ifndef ACTUATOR_CONTROLLER_H
#define ACTUATOR_CONTROLLER_H

#include <Arduino.h>
#include "DCPump.h"
#include "PeristalticPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"

class ActuatorController {
public:
    static void initialize(DCPump& airPump, DCPump& drainPump,
                           PeristalticPump& nutrientPump, PeristalticPump& basePump,
                           StirringMotor& stirringMotor, HeatingPlate& heatingPlate,
                           LEDGrowLight& ledGrowLight);
    static void beginAll();
    
    static void runActuator(const String& actuatorName, float value, int duration);
    static void stopActuator(const String& actuatorName);
    static void stopAllActuators();
    static bool isActuatorRunning(const String& actuatorName);

    template<typename T>
    static float getPumpParameter(const String& actuatorName, float (T::*getter)() const);

    static float getVolumeAdded(const String& actuatorName);
    static float getVolumeRemoved(const String& actuatorName);
    static void resetVolumeAdded(const String& actuatorName);
    static void resetVolumeRemoved(const String& actuatorName);
    static float getTotalVolumeAdded();
    static float getTotalVolumeRemoved();

    static float getPumpMaxFlowRate(const String& actuatorName);
    static float getPumpMinFlowRate(const String& actuatorName);
    static int getStirringMotorMinRPM();
    static int getStirringMotorMaxRPM();

    static ActuatorInterface* findActuatorByName(const String& name);

private:
    static DCPump* airPump;
    static DCPump* drainPump;
    static PeristalticPump* nutrientPump;
    static PeristalticPump* basePump;
    static StirringMotor* stirringMotor;
    static HeatingPlate* heatingPlate;
    static LEDGrowLight* ledGrowLight;
};

#endif // ACTUATOR_CONTROLLER_H

