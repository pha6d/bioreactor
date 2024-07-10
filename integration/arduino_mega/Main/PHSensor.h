/*
 * PHSensor.h
 * This file defines a class for reading pH value from a pH sensor.
 * The class implements the SensorInterface and provides methods to initialize the sensor, read the pH value, and perform calibration.
 *
 * Physical modules used:
 * - Gravity: Analog pH Meter V2
 */

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

#ifndef PHSENSOR_H
#define PHSENSOR_H

#include "SensorInterface.h"
#include "DFRobot_PH.h"
#include <EEPROM.h>
#include <Arduino.h>

class PHSensor : public SensorInterface {
public:
    /*
     * Constructor for PHSensor.
     * @param pin: The analog pin connected to the pH sensor.
     */
    PHSensor(int pin);

    /*
     * Method to initialize the pH sensor.
     */
    void begin();

    /*
     * Method to read the pH value from the sensor.
     * @return: The pH value.
     */
    float readValue();

    /*
     * Method to perform pH calibration.
     * @param voltage: The voltage read from the pH sensor.
     * @param temperature: The current temperature for compensation.
     */
    void calibrate(float voltage, float temperature);

private:
    int _pin;          // Analog pin connected to the pH sensor
    DFRobot_PH _ph;    // DFRobot_PH object for pH calculation
    float _voltage;    // Voltage read from the pH sensor
    float _temperature; // Current temperature for compensation
};

#endif


/*
Installation Instructions:

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
*/
