/*
 * MixProgram.h
 * Header file for the MixProgram class. This class controls the mixing process of the bioreactor.
 * It allows starting, updating, and checking the status of the mixing process.
 */

#ifndef MIX_PROGRAM_H
#define MIX_PROGRAM_H

#include "StirringMotor.h"

 // MixProgram class definition
class MixProgram {
public:
    // Initializes the mixing process with the specified stirring motor and speed
    void begin(StirringMotor& motor, int speed);

    // Updates the mixing process, checking if it should continue or stop
    void update();

    // Checks if the mixing process is currently running
    bool isRunning();

private:
    StirringMotor* stirringMotor;  // Pointer to the stirring motor
    int speed;                     // Speed at which the motor should operate
    bool running;                  // Indicates if the mixing process is running
};

#endif // MIX_PROGRAM_H
