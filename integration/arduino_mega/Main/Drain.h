/*
 * Drain.h
 * Header file for the DrainProgram class. This class controls the drain process of the bioreactor.
 * It allows starting, updating, and checking the status of the drain process.
 */

#ifndef DRAIN_H
#define DRAIN_H

#include "DCPump.h"

 // DrainProgram class definition
class DrainProgram {
public:
    // Initializes the drain process with the specified pump, rate, and duration
    void begin(DCPump& pump, int rate, int duration);

    // Updates the drain process, checking if it should continue or stop
    void update();

    // Checks if the drain process is currently running
    bool isRunning();

private:
    DCPump* drainPump;  // Pointer to the drain pump
    int rate;           // Rate at which the pump should operate
    int duration;       // Duration of the drain process in seconds
    unsigned long startTime;  // Start time of the drain process
    bool running;       // Indicates if the drain process is running
};

#endif // DRAIN_H
