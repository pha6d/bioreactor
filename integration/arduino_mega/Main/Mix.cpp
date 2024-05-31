#include "Mix.h"
#include <SoftwareSerial.h> 

// Implementation of the function to start mixing at a certain speed
void runMix(StirringMotor& stirringMotor, int speed) {
    stirringMotor.control(true, speed);
    Serial.println(), Serial.println("run MIX program");
    Serial.print("Mixing started at speed: ");
    Serial.println(speed);
}

