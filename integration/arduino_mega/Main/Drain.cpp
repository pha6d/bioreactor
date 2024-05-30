#include "Drain.h"

// Implementation of the function to start draining with a specified rate and duration
void runDrain(DCPump& drainPump, int rate, int duration) {
    Serial.println("Draining started...");
    drainPump.control(true, rate); // Start the drain pump at specified speed

    delay(duration * 1000); // Convert duration to milliseconds and wait

    drainPump.control(false, 0); // Stop the drain pump
    Serial.print("Draining at rate ");
    Serial.print(rate);
    Serial.print("% for ");
    Serial.print(duration);
    Serial.println(" seconds completed.");
}
