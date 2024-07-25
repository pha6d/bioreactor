#include "ActuatorController.h"
#include <Arduino.h>

// External actuator declarations
extern PeristalticPump nutrientPump;
extern PeristalticPump basePump;
extern DCPump airPump;
extern DCPump drainPump;
extern StirringMotor stirringMotor;
extern HeatingPlate heatingPlate;
extern LEDGrowLight ledGrowLight;



// Ces méthodes ne sont pas utilisées dans l'implémentation actuelle
// mais sont conservées pour une utilisation potentielle future.
// Elles pourraient être utilisées pour initialiser les actuateurs
// dans les différents programmes ou pour des tests plus spécifiques.

// The following methods are not used in the current implementation but are kept for potential future use
void ActuatorController::runPeristalticPump(PeristalticPump& pump, float flowRate, int duration) {
    logActuatorOperation(pump.getName(), flowRate, duration);
    pump.control(true, flowRate);
    delay(duration * 1000);
    pump.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runDCPump(DCPump& pump, int speed, int duration) {
    logActuatorOperation(pump.getName(), speed, duration);
    pump.control(true, speed);
    delay(duration * 1000);
    pump.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runStirringMotor(StirringMotor& motor, int speed, int duration) {
    logActuatorOperation("Stirring Motor", speed, duration);
    motor.control(true, speed);
    delay(duration * 1000);
    motor.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runHeatingPlate(HeatingPlate& heatingPlate, int temperature, int duration) {
    logActuatorOperation("Heating Plate", temperature, duration);
    heatingPlate.control(true, temperature);
    delay(duration * 1000);
    heatingPlate.control(false, 0);
    Serial.println("Operation completed");
}

void ActuatorController::runLEDGrowLight(LEDGrowLight& light, int intensity, int duration) {
    logActuatorOperation("LED Grow Light", intensity, duration);
    light.control(true, intensity);
    delay(duration * 1000);
    light.control(false, 0);
    Serial.println("Operation completed");
}