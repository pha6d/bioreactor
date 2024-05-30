#ifndef DRAIN_H
#define DRAIN_H

#include "DCPump.h"

// Function to start draining with a specified rate and duration
void runDrain(DCPump& drainPump, int rate, int duration);

#endif
