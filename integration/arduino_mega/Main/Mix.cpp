/*
 * Mix.cpp
 * This file manages the mixing program using the MixProgram class.
 * It periodically checks if the stopFlag is set and stops the stirring motor if necessary.
 */

#include "Mix.h"

 // External declarations to share global variables
extern bool stopFlag;
extern String currentProgram;
extern String programStatus;

// Initialization of the mixing program
void MixProgram::begin(StirringMotor& motor, int speed) {
    this->stirringMotor = &motor;  // Set the stirring motor
    this->speed = speed;           // Set the motor speed
    this->running = true;          // Set the running state to true
    stopFlag = false;              // Reset the stop flag
    currentProgram = "Mix";        // Set the name of the current program
    programStatus = "Running";     // Set the program status to "Running"
    Serial.println("Mixing started at speed: " + String(speed)); // Log the start of the mixing process
}

// Update the mixing program
void MixProgram::update() {
    if (!running) return;  // If the program is not running, exit the function

    if (stopFlag) {  // Check if the stop flag is set
        Serial.println("Mixing interrupted.");
        programStatus = "Stopped";  // Update the program status
        running = false;  // Stop the program
        stirringMotor->control(false, 0);  // Stop the stirring motor
        Serial.println("Stirring Motor is OFF");
        return;  // Exit the function
    }

    stirringMotor->control(true, speed);  // Continue running the stirring motor at the specified speed
    Serial.println("Stirring Motor is ON, Speed set to: " + String(speed));
}

// Check if the mixing program is running
bool MixProgram::isRunning() {
    return running;
}
