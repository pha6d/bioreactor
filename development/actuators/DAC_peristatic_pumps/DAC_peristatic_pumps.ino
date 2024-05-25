/*
 * Control Setup for Peristaltic Pump using Arduino and MCP4725 DAC
 *
 * Overview:
 * This setup uses an Arduino board connected to a MCP4725 Digital-to-Analog Converter (DAC)
 * to control the flow rate of a peristaltic pump (model KK300) via the KMD motor driver and
 * the 2802 control board. The flow rate is controlled by adjusting the voltage output from
 * the MCP4725, which is interpreted by the 2802 board to adjust the pump speed.
 *
 * Wiring Description:
 * 1. Arduino to MCP4725:
 *    - Connect Arduino SDA (A4 on Uno, 20 on Mega) to MCP4725 SDA.
 *    - Connect Arduino SCL (A5 on Uno, 21 on Mega) to MCP4725 SCL.
 *    - Connect Arduino GND to MCP4725 GND.
 *    - Connect Arduino 5V to MCP4725 VCC.
 *    This setup allows the Arduino to communicate with the MCP4725 via I2C protocol.
 *
 * 2. MCP4725 to 2802 Control Board:
 *    - Connect MCP4725 Vout to 2802 control input (check 2802 manual for the specific pin).
 *    This connection sends the voltage signal from the MCP4725 to the 2802 board, which controls
 *    the speed of the pump based on the voltage level.
 *
 * 3. 2802 Control Board to KMD Motor Driver:
 *    - Connect specified 2802 output pins to KMD driver input pins according to their manuals.
 *    This step transmits control signals from the 2802 board to the KMD driver, which directly
 *    drives the pump motor.
 *
 * 4. KMD Driver to KK300 Peristaltic Pump:
 *    - Connect KMD output terminals to the KK300 motor terminals.
 *    This final connection allows the KMD driver to operate the peristaltic pump motor,
 *    enabling precise control of the fluid flow rate based on the input from the Arduino.
 *
 * Purpose:
 * The purpose of this setup is to allow precise control of a peristaltic pump's flow rate for
 * applications such as automated dispensing, chemical dosing in labs, or any application
 * where fluid flow needs to be precisely controlled and varied.
 *
 * The DAC (MCP4725) is used here because it can provide a precise and stable analog output voltage
 * that can be easily controlled via the Arduino's digital commands, making it ideal for
 * integrating into electronic control systems that require analog inputs.
 */

// https://github.com/adafruit/Adafruit_MCP4725
// https://passionelectronique.fr/tutorial-mcp4725/
// Install "Adafruit MCP4725" library
// read Kaomer pump data sheets : 2802 user's manual_V1.2, KMD-42B-P two-phase DC stepping driver product manual_ v1.3, KK300 Product Manual A0


#include <Wire.h>
#include <Adafruit_MCP4725.h>

// Initialize two MCP4725 DAC objects for two different pumps
Adafruit_MCP4725 dacNutrients;
Adafruit_MCP4725 dacBase;

// Desired flow rates in ml/min for each pump
const float desiredFlowRateNutrients = 10.0;  // Example flow rate for nutrients pump
const float desiredFlowRateBase = 0.0;       // Example flow rate for base pump

// These values should be determined based on your specific setup and calibration needs
const float minFlowRate = 0.5;    // Minimum flow rate the pumps can handle (ml/min)
const float maxFlowRate = 105.0;  // Maximum flow rate the pumps can handle (ml/min)
const uint16_t dacMin = 0;        // Minimum DAC output corresponding to minimum flow rate
const uint16_t dacMax = 4095;     // Maximum DAC output corresponding to maximum flow rate

void setup() {
  Serial.begin(9600);
  // Initialize the MCP4725 DACs at different I2C addresses if available
  dacNutrients.begin(0x60);  // Default I2C address of the MCP4725 for nutrients pump
  dacBase.begin(0x61);       // Another possible I2C address for the base pump

  // Calculate DAC values for desired flow rates
  uint16_t dacValueNutrients = mapFlowRateToDAC(desiredFlowRateNutrients, minFlowRate, maxFlowRate, dacMin, dacMax);
  uint16_t dacValueBase = mapFlowRateToDAC(desiredFlowRateBase, minFlowRate, maxFlowRate, dacMin, dacMax);

  // Set the DAC output voltages to control the pumps
  dacNutrients.setVoltage(dacValueNutrients, false);
  dacBase.setVoltage(dacValueBase, false);
}

void loop() {
  // No need to repeat the control in the loop unless there is a need for dynamic adjustment
  delay(1000);  // Delay for stability, not necessary unless dynamically changing values
}

// This function maps the desired flow rate to a DAC output value
uint16_t mapFlowRateToDAC(float flowRate, float minFlow, float maxFlow, uint16_t dacMin, uint16_t dacMax) {
  if (flowRate < minFlow) flowRate = minFlow;
  else if (flowRate > maxFlow) flowRate = maxFlow;

  // Map the flow rate linearly to the DAC range
  return (uint16_t)map(flowRate, minFlow, maxFlow, dacMin, dacMax);
}

// The map function re-maps a number from one range to another
// that is, a value of fromLow would get mapped to toLow, a value of fromHigh to toHigh
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

