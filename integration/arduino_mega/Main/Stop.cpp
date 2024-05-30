#include "Stop.h"

void runStop(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight) {
    airPump.control(false, 0);
    drainPump.control(false, 0);
    nutrientPump.control(false, 0);
    basePump.control(false, 0);
    stirringMotor.control(false, 0);
    heatingPlate.control(false);
    ledGrowLight.control(false);
    Serial.println("All actuators stopped.");
}
