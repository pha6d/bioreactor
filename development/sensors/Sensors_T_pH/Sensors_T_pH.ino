
/*
Installation Instructions:

For the DS18B20 Temperature Sensor:
@https://wiki.dfrobot.com/Waterproof_DS18B20_Digital_Temperature_Sensor__SKU_DFR0198_
1. Connect the DS18B20 to the Arduino using a plugable terminal sensor adapter or directly by wiring.
    - Red Wire (VCC) to 5V or 3.3V on the Arduino.
    - Black Wire (GND) to GND on the Arduino.
    - Blue Wire (Data Signal) to a digital pin on the Arduino (Pin 52 in this case).
2. Use "Gravity: Terminal Sensor Adapter V2.0" @https://www.dfrobot.com/product-203.html
        or Place a 4.7kΩ pull-up resistor between the VCC and Data Signal lines to ensure proper communication.

For the Gravity: Analog pH Meter V2:
@https://www.dfrobot.com/product-1782.html
1. Connect the pH meter's BNC to the pH probe and the board to the Arduino.
    - Signal Pin to an analog pin on the Arduino (A1 in this case).
    - V+ to 5V on the Arduino.
    - GND to GND on the Arduino.
2. Before taking measurements, calibrate the pH meter with standard buffer solutions.
*/



#include <OneWire.h>  // Include the OneWire library for DS18B20 temperature sensor
#include "DFRobot_PH.h"  // Include the DFRobot pH library for pH meter functionality
#include <EEPROM.h>  // Include the EEPROM library for saving calibration data

// Pin for DS18B20 temperature sensor
int DS18S20_Pin = 52; // Define the digital pin for the DS18S20 temperature sensor

// Initialize the OneWire library with the DS18S20 pin
OneWire ds(DS18S20_Pin);

// Pin connected to the pH sensor
#define PH_PIN A1  // Define the analog pin for the pH sensor

float voltage, phValue, temperature;  // Variables to store voltage, pH value, and temperature
DFRobot_PH ph;  // Create an instance of the DFRobot_PH class

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  ph.begin();  // Initialize the pH meter
}

void loop() {
  static unsigned long timepoint = millis();
  // Take measurements every 1000 milliseconds (1 second)
  if(millis() - timepoint > 1000U) { 
    timepoint = millis();

    // Read the temperature from the DS18B20 sensor
    temperature = readTemperature(); // Get temperature for pH temperature compensation
    
    // Read the voltage from the pH sensor
    voltage = analogRead(PH_PIN) / 1024.0 * 5000;  // Convert analog reading to millivolts
    phValue = ph.readPH(voltage, temperature);  // Calculate pH value with temperature compensation
    
    // Print the temperature and pH values to the Serial Monitor
    Serial.print("temperature: ");
    Serial.print(temperature, 1);  // Print temperature with one decimal place
    Serial.print("°C  pH: ");
    Serial.println(phValue, 2);  // Print pH value with two decimal places
  }
  // Check for calibration commands via Serial and perform calibration
  ph.calibration(voltage, temperature); 
}

// Function to get temperature from the DS18B20 sensor
float readTemperature() {
  byte data[12];  // Array to store temperature data
  byte addr[8];  // Array to store sensor address

  // Search for sensors on the bus
  if (!ds.search(addr)) {
    ds.reset_search();  // Reset search for the next attempt
    return -1000; // Return a placeholder indicating no sensor found
  }

  // Check if data received is valid
  if (OneWire::crc8(addr, 7) != addr[7]) {
    Serial.println("CRC is not valid!");  // Print error message
    return -1000;
  }

  // Verify the sensor type is DS18B20
  if (addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.print("Device is not recognized");  // Print error message
    return -1000;
  }

  // Reset, select the sensor, then initiate temperature conversion
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1); // Start conversion, with parasite power on at the end

  // Wait for conversion to complete then read the scratchpad
  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Command to read Scratchpad

  // Read the temperature data
  for (int i = 0; i < 9; i++) { // We need 9 bytes as per DS18B20 specs
    data[i] = ds.read();
  }

  ds.reset_search(); // Reset the search to allow us to detect the next device in the next loop

  // Processing the data received from the sensor
  byte MSB = data[1]; // Most Significant Byte of the temperature
  byte LSB = data[0]; // Least Significant Byte of the temperature
  
  // Forming the raw temperature value from two bytes
  float tempRead = ((MSB << 8) | LSB); // Use bitwise OR to combine MSB and LSB
  
  // Converting raw value to Celsius
  float TemperatureSum = tempRead / 16;
  
  return TemperatureSum; // Return the processed temperature
}