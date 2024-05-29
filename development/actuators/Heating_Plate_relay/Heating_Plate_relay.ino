/*
  Instructions to connect the relay and control a 12V heating plate with Arduino:

  1. Connections for the relay to Arduino:
     - Relay VCC: Connect to 5V on the Arduino.
     - Relay GND: Connect to GND on the Arduino.
     - Relay IN (control signal): Connect to digital pin 13 on the Arduino.

  2. Connections for the heating plate to the relay:
     - Heating plate + (positive): Connect to the normally open (NO) terminal of the relay.
     - Heating plate - (negative): Connect to the 12V power supply ground.
     - 12V power supply + (positive): Connect to the common (COM) terminal of the relay.

  This code will turn the heating plate on or off based on commands received from the serial monitor.
*/

const int relayPin = 13; // Pin connected to the relay

void setup() {
  Serial.begin(115200);  // Start the serial communication at 115200 baud rate
  pinMode(relayPin, OUTPUT); // Set the relay pin as an output
  digitalWrite(relayPin, LOW); // Ensure the relay is off at the start
  Serial.println("Heating Plate Control Initialized.");
  Serial.println("Type 'on' to turn on the heating plate.");
  Serial.println("Type 'off' to turn off the heating plate.");
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Read the incoming command
    command.trim(); // Remove any trailing whitespace

    if (command.equalsIgnoreCase("on")) {
      digitalWrite(relayPin, HIGH); // Turn on the relay (and the heating plate)
      Serial.println("Heating plate turned ON.");
    } else if (command.equalsIgnoreCase("off")) {
      digitalWrite(relayPin, LOW); // Turn off the relay (and the heating plate)
      Serial.println("Heating plate turned OFF.");
    } else {
      Serial.println("Invalid command. Type 'on' or 'off'.");
    }
  }
}
