#include <SPI.h>
#include <Adafruit_MAX31865.h>

// SDI (Serial Data Input): This is equivalent to MOSI (Master Out Slave In) on the Arduino. Connect this to pin 11 on the Arduino Uno.
// SDO (Serial Data Output): This is equivalent to MISO (Master In Slave Out) on the Arduino. Connect this to pin 12 on the Arduino Uno.
// CLK (Clock): This pin should be connected to the SCK (Serial Clock) pin, which is pin 13 on the Arduino Uno.
// CS (Chip Select): This pin can be connected to any digital I/O pin on the Arduino. For this example, we'll use pin 10.
// DRDY (Data Ready): This pin is an output from the MAX31865 that signals when data is ready to be read. It's not mandatory for basic operations but can be connected to another digital I/O pin if you wish to use it for interrupt-driven reads.

// Define the CS pin for the MAX31865 module
#define MAX31865_CS 2

// Create an instance of the MAX31865 library
Adafruit_MAX31865 rtd = Adafruit_MAX31865(MAX31865_CS);

void setup() {
  Serial.begin(115200);  // Start the serial communication at 115200 baud rate
  rtd.begin(MAX31865_3WIRE);  // Set up the MAX31865 module for 3-wire RTD
}

void loop() {
  // Read the temperature
  float temperature = rtd.temperature(100, 430); // Parameters are Rref and RTD nominal resistance
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C");

  // Check for faults
  checkFault();

  delay(1000);  // Wait for one second before reading again
}

void checkFault() {
  uint8_t fault = rtd.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold");
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold");
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias");
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - RTD disconnected");
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage");
    }
    rtd.clearFault();
  }
}
