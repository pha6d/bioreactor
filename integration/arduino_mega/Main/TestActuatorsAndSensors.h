/*
 * TestActuatorsAndSensors.h
 * This file declares the function to test all actuators and sensors in the bioreactor system.
 * 
 * The test involves activating each actuator (pumps, motors, heaters, lights) in sequence and 
 * reading from each sensor to ensure they are functioning correctly.
 */

#ifndef TEST_ACTUATORS_AND_SENSORS_H
#define TEST_ACTUATORS_AND_SENSORS_H

// Include interfaces
#include "ActuatorInterface.h"
#include "SensorInterface.h"

// Include actuators
#include "PeristalticPump.h"
#include "DCPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"

// Include sensors
#include "PHSensor.h"
#include "OxygenSensor.h"
#include "TurbiditySensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PT100Sensor.h"
#include "AirFlowSensor.h"

/*
 * Function to run a test on all actuators and sensors.
 * 
 * This function activates each actuator one by one, waits for a few seconds, and then deactivates it.
 * It also reads from each sensor and prints the values to the Serial monitor.
 */
void runTestActuatorsAndSensors(DCPump &airPump, DCPump &drainPump, StirringMotor &stirringMotor, PeristalticPump &nutrientPump, PeristalticPump &basePump, HeatingPlate &heatingPlate, LEDGrowLight &ledGrowLight, PT100Sensor &waterTempSensor, DS18B20TemperatureSensor &airTempSensor, PHSensor &phSensor, TurbiditySensor &turbiditySensor, OxygenSensor &oxygenSensor, AirFlowSensor &airFlowSensor);

#endif
