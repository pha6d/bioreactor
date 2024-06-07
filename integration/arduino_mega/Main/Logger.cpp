/*
 * Logger.cpp
 * This file provides the implementation of the logData function which logs the data from various sensors and actuators.
 */

#include "Logger.h"
#include <SoftwareSerial.h> // Include SoftwareSerial

extern SoftwareSerial espSerial; // Ensure the extern declaration is here

void logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, 
             StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight, 
             PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, 
             TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
             const String& experimentName, const String& comment, const String& programStatus) {
                 
    String data = "{";
    data += "\"experimentName\":\"" + experimentName + "\",";
    data += "\"comment\":\"" + comment + "\",";
    data += "\"programStatus\":\"" + programStatus + "\",";
    data += "\"airPumpStatus\":" + String(airPump.isOn()) + ",";
    data += "\"drainPumpStatus\":" + String(drainPump.isOn()) + ",";
    data += "\"nutrientPumpStatus\":" + String(nutrientPump.isOn()) + ",";
    data += "\"basePumpStatus\":" + String(basePump.isOn()) + ",";
    data += "\"stirringMotorStatus\":" + String(stirringMotor.isOn()) + ",";
    data += "\"heatingPlateStatus\":" + String(heatingPlate.isOn()) + ",";
    data += "\"ledGrowLightStatus\":" + String(ledGrowLight.isOn()) + ",";
    data += "\"waterTemp\":" + String(waterTempSensor.readValue()) + ",";
    data += "\"airTemp\":" + String(airTempSensor.readValue()) + ",";
    data += "\"ph\":" + String(phSensor.readValue()) + ",";
    data += "\"turbidity\":" + String(turbiditySensor.readValue()) + ",";
    data += "\"oxygen\":" + String(oxygenSensor.readValue()) + ",";
    data += "\"airFlow\":" + String(airFlowSensor.readValue());
    data += "}";

    espSerial.println(data);
    Serial.println(data); // Also print the data to the Serial monitor for debugging
}
