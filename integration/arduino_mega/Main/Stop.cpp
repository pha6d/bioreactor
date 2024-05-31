#include "Stop.h"

// Include extern declarations
extern bool stopFlag;

void runStop(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight) {
    stopFlag = true; // Set the stop flag to true

    // Stop all actuators
    airPump.control(false, 0);
    drainPump.control(false, 0);
    nutrientPump.control(false, 0);
    basePump.control(false, 0);
    stirringMotor.control(false, 0);
    heatingPlate.control(false, 0);
    ledGrowLight.control(false);
    Serial.println(), Serial.println("run STOP program");
    
}
