// Logger.cpp
#include "Logger.h"

extern SoftwareSerial espSerial;

void logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight, PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor) {
    float waterTemp = waterTempSensor.readValue();
    float airTemp = airTempSensor.readValue();
    float pH = phSensor.readValue();
    float turbidity = turbiditySensor.readValue();
    float oxygen = oxygenSensor.readValue();
    float airFlow = airFlowSensor.readValue();

    String data = String(waterTemp) + "," + String(airTemp) + "," + String(pH) + "," + String(turbidity) + "," + String(oxygen) + "," + String(airFlow);
    espSerial.println(data); // Utilisation de la variable externe
    Serial.println("Logged data: " + data);
}

