/*
 * Drain.cpp
 * This file manages the drain program using the DrainProgram class.
 * It periodically checks if the stopFlag is set and stops the drain pump if necessary.
 */

#include "Drain.h"

// External declarations to share global variables
extern bool stopFlag;
extern String currentProgram;
extern String programStatus;

// Initialization of the drain program
void DrainProgram::begin(DCPump& pump, int rate, int duration) {
    this->drainPump = &pump;  // Set the drain pump
    this->rate = rate;        // Set the pump rate
    this->duration = duration;  // Set the drain process duration
    this->startTime = millis();  // Record the start time
    this->running = true;     // Set the running state to true
    stopFlag = false;         // Reset the stop flag
    currentProgram = "Drain"; // Set the name of the current program
    programStatus = "Running"; // Set the program status to "Running"
    Serial.println("Drain started at speed: " + String(rate)); // Log the start of the drain process
    drainPump->control(true, rate);  // Start the drain pump
    Serial.println("Drain Pump is ON, Speed set to: " + String(rate)); // Log pump start
}

// Update the drain program
void DrainProgram::update() {
    if (!running) return;  // If the program is not running, exit the function

    if (stopFlag) {  // Check if the stop flag is set
        Serial.println("Drain interrupted.");
        programStatus = "Stopped";  // Update the program status
        running = false;  // Stop the program
        drainPump->control(false, 0);  // Stop the drain pump
        Serial.println("Drain Pump is OFF");
        return;  // Exit the function
    }

    if (millis() - startTime >= duration * 1000) {  // Check if the drain duration has elapsed
        drainPump->control(false, 0);  // Stop the drain pump
        programStatus = "Completed";  // Update the program status
        running = false;  // Stop the program
        Serial.println("Drain finished.");
        Serial.println("Drain Pump is OFF");
    }
}

// Check if the drain program is running
bool DrainProgram::isRunning() {
    return running;
}
