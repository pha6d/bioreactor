#include <Arduino.h>

// FAN : be quiet! Pure Wings 2 PWM 120 mm 1500rpm (4 pins)
// The tachometer pin could not be used because the voltage was too low in the state to be read by Arduino.
// Note: The fan cannot operate below 390 RPM, so a relay is used to completely turn it off.

const int pwmPin = 9;    // PWM control pin connected to the "I" wire of the fan
const int relayPin = 10;  // Relay control pin

void setup() {
  pinMode(pwmPin, OUTPUT);  // Set the PWM pin as an output
  pinMode(relayPin, OUTPUT); // Set the relay pin as an output
  Serial.begin(115200);     // Start serial communication at 115200 baud rate

  int targetRPM = 1500;  // Target fan speed in RPM
  controlFan(true, targetRPM);  // Control fan to reach target RPM
}

void loop() {
  // Example: Run the fan for 5 seconds at 1500 RPM, then turn off for 5 seconds
  controlFan(true, 1500);
  delay(5000);
  controlFan(false, 0);
  delay(7000);
}

// Function to control the fan with on/off and target RPM settings
void controlFan(bool state, int targetRPM) {
  if (state && targetRPM >= 390) {
    int pwmValue = rpmToPWM(targetRPM);  // Calculate corresponding PWM value for the target RPM
    analogWrite(pwmPin, pwmValue);       // Apply the PWM value to the fan
    digitalWrite(relayPin, HIGH);        // Turn on the relay
    Serial.print("Fan is ON, RPM set to: ");
    Serial.print(targetRPM);
    Serial.print(" corresponds to PWM value: ");
    Serial.println(pwmValue);
  } else {
    analogWrite(pwmPin, 0);              // Set the PWM value to 0
    digitalWrite(relayPin, LOW);         // Turn off the relay
    Serial.println("Fan is OFF");
  }
}

// Function to convert RPM to PWM value
int rpmToPWM(int targetRPM) {
  float loadPercentage;  // Variable to hold the calculated load percentage

  // Calculate the load percentage based on the target RPM using piecewise linear equations
  if (targetRPM <= 450) {
    // First equation for RPMs from 0% to 32% load
    loadPercentage = (targetRPM - 390) / 1.875;
  } else {
    // Second equation for RPMs from 32% to 100% load
    loadPercentage = (targetRPM - 450) / 15.441 + 32;
  }

  // Convert the load percentage to a PWM value (range 0 to 255)
  int pwmValue = map(int(loadPercentage), 0, 100, 0, 255);
  return pwmValue;
}
