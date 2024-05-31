#include "Drain.h"

// Include extern declarations
extern bool stopFlag;

void runDrain(DCPump& drainPump, int rate, int duration) {
    Serial.println("run DRAIN program");
    unsigned long startTime = millis();
    stopFlag = false;  // Reset stopFlag at the start of the Drain process
    drainPump.control(true, rate); // Start the pump with the specified rate

    while ((millis() - startTime) < (duration * 1000)) {
        if (stopFlag) {
            break; // Exit the loop if stop is requested
        }
        delay(100); // Small delay to prevent CPU overload
    }

    drainPump.control(false, 0); // Stop the pump after the specified duration or if stopped
}
