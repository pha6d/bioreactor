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
@https://wiki.dfrobot.com/Gravity__Analog_pH_Sensor_Meter_Kit_V2_SKU_SEN0161-V2
1. Connect the pH meter's BNC to the pH probe and the board to the Arduino.
    - Signal Pin to an analog pin on the Arduino (A1 in this case).
    - V+ to 5V on the Arduino.
    - GND to GND on the Arduino.
2. Before taking measurements, calibrate the pH meter with standard buffer solutions. @https://wiki.dfrobot.com/Gravity__Analog_pH_Sensor_Meter_Kit_V2_SKU_SEN0161-V2
    -1. Input enterph command in the serial monitor to enter the calibration mode.
    -2. Input calph commands in the serial monitor to start the calibration. The program will automatically identify which of the two standard buffer solutions is present: either 4.0 and 7.0. In this step, the standard buffer solution of 7.0 will be identified.
    -3. After the calibration, input exitph command in the serial monitor to save the relevant parameters and exit the calibration mode. Note: Only after inputing exitph command in the serial monitor can the relevant parameters be saved.
    -4. After the above steps, the first point calibration is completed. The second point calibration will be performed below.
          -Wash the probe with distilled water, then absorb the residual water-drops with paper. Insert the pH probe into the standard buffer solution of 4.0, stir gently, until the values are stable.
          -After the values are stable, the second point can be calibrated. These steps are the same as the first calibration step. The specific steps are as follows:enterph, calph, exitph
    -5. After completing the above steps, the two-point calibration is completed, and then the sensor can be used for actual measurement. The relevant parameters in the calibration process have been saved to the EEPROM of the main control board
ATTENTION : After completing the measurement, disconnect the pH probe from the signal conversion board. The pH probe should not be connected to the signal conversion board without the power supply for a long time.

For the Gravity: Analog Turbidity Sensor
@https://wiki.dfrobot.com/Turbidity_sensor_SKU__SEN0189
1. Connect the sensor output to an analog pin on the Arduino (A2 in this case).
2. Connect V+ to 5V on the Arduino and GND to GND.

For Gravity: Analog Dissolved Oxygen Sensor / Meter Kit for Arduino
@https://wiki.dfrobot.com/Gravity__Analog_Dissolved_Oxygen_Sensor_SKU_SEN0237
1. Connect the DO sensor to the Arduino board:
    - Signal Pin to an analog pin on the Arduino (A1 in this case).
    - V+ to 5V on the Arduino.
    - GND to GND on the Arduino.
2. Please ensure that the sensor is calibrated in air before the first use to ensure accurate readings. Submerge the sensor in water and wait several minutes for the readings to stabilize before calibration:
    -Use a stirrer or an eggbeater to continuously stir or Use an air pump to continuously inflate the water for 10 minutes to saturate the dissolved oxygen). To obtain two points, perform this procedure with one water cooled in the fridge and the other at room temperature or heated.
    -Add sodium sulfite(Na2SO3) into water until it is saturated. This can consume all the oxygen in the water to obtain the zero dissolved oxygen liquid.
3. For calibration in the air, expose the membrane to the air and set the output value to 100% oxygen saturation (can be done via software using calibration commands or physically via the potentiometer on the sensor board).
4. The actual DO measurement will vary with temperature. It is highly recommended to use a temperature compensation function which can be achieved by integrating a temperature sensor like the DS18B20 to provide real-time temperature data for more accurate DO readings.
*/



#include <OneWire.h>  // Include the OneWire library for DS18B20 temperature sensor. Please install the library from Arduino IDE
#include "DFRobot_PH.h"  // Include the DFRobot pH library for pH meter functionality. Please download DFRobot_PH Library @https://github.com/DFRobot/DFRobot_PH/archive/master.zip
#include <EEPROM.h>  // Include the EEPROM library for saving calibration data
#include <Arduino.h>

// Pin for DS18B20 temperature sensor
int DS18S20_Pin = 52; // Define the digital pin for the DS18S20 temperature sensor

// Initialize the OneWire library with the DS18S20 pin
OneWire ds(DS18S20_Pin);

// Pin connected to the pH sensor
#define PH_PIN A1  // Define the analog pin for the pH sensor

// Pin connected to the turbidity sensor
#define TURBIDITY_PIN A2 // Define the analog pin for the turbidity sensor

// Pin connected to the dissolved oxygen sensor-> remettre en A4
#define DO_PIN A1 

float voltage, phValue, temperature;  // Variables to store voltage, pH value, and temperature
DFRobot_PH ph;  // Create an instance of the DFRobot_PH class

// Dissolved oxygen sensor calibration and initialisation code :
// Reference voltage in millivolts and ADC resolution
#define VREF 5000    // Reference voltage (millivolts)
#define ADC_RES 1024 // ADC Resolution

// Calibration mode selection: 0 for single-point, 1 for two-point
#define TWO_POINT_CALIBRATION 1 // select 1 or 0

//Single point calibration needs to be filled CAL1_V and CAL1_T
#define CAL1_V (1600) //mv
#define CAL1_T (25)   //℃
//Two-point calibration needs to be filled CAL2_V and CAL2_T
//CAL1 High temperature point, CAL2 Low temperature point
#define CAL2_V (1300) //mv
#define CAL2_T (15)   //℃

// Table of saturated dissolved oxygen levels for different temperatures
const uint16_t DO_Table[41] = {
  // Values from 0°C to 40°C
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410
    };

uint16_t ADC_Raw_O2;     // Variable to store raw ADC value
uint16_t ADC_Voltage_O2; // Variable to store the calculated voltage
uint16_t DO;          // Variable to store the dissolved oxygen concentration

// Function to calculate dissolved oxygen concentration based on voltage and temperature
int16_t readDO(uint32_t voltage_mv, uint8_t temperature_c) {
  uint16_t V_saturation;
#if TWO_POINT_CALIBRATION == 0
  // Calculate V_saturation for single-point calibration
  V_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperature_c - (uint32_t)CAL1_T * 35;
#else
  // Calculate V_saturation for two-point calibration
  V_saturation = (int16_t)((int8_t)temperature_c - CAL2_T) * ((uint16_t)CAL1_V - CAL2_V) / ((uint8_t)CAL1_T - CAL2_T) + CAL2_V;
#endif
  return (voltage_mv * DO_Table[temperature_c] / V_saturation); // Calculate DO concentration
}


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
    Serial.print(phValue, 2);  // Print pH value with two decimal places

    // Read and print the turbidity sensor value
    int sensorValue = analogRead(TURBIDITY_PIN); // Read the input on analog pin 2
    float turbidityVoltage = sensorValue * (5.0 / 1024.0); // Convert to voltage
    Serial.print("  Turbidity Voltage: ");
    Serial.print(turbidityVoltage); // Print turbidity voltage

    //Read and print O2 sensor values
    ADC_Raw_O2 = analogRead(DO_PIN); // Read the analog value from DO sensor
    ADC_Voltage_O2 = uint32_t(VREF) * ADC_Raw_O2 / ADC_RES; // Convert ADC value to voltage

    // Print temperature, raw ADC value, calculated voltage, and DO concentration
    //Serial.print("O2 ADC RAW:\t" + String(ADC_Raw_O2) + "\t");
    //Serial.print("O2 ADC Voltage:\t" + String(ADC_Voltage_O2) + "\t");
    Serial.print("  DO: ");
    Serial.print(readDO(ADC_Voltage_O2, temperature), 1); // Print DO concentration with 1 decimal places
    Serial.print(" mg/L ");
    Serial.println(); // Move to the next line after printing all sensor values

  }
  // Check for calibration commands via Serial and perform calibration
  ph.calibration(voltage, temperature); 

  delay(1000); // Wait for 1 second before the next loop iteration
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