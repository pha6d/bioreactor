#include "Logger.h"
#include <SoftwareSerial.h> 

extern SoftwareSerial espSerial;

void logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump,
    StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor,
    TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
    const String& experimentName, const String& comment, const String& currentProgram) {

    String data = "{\"Experiment\": \"" + experimentName + "\", \"Comment\": \"" + comment + "\", ";
    data += "\"Current_Program\": \"" + currentProgram + "\", ";
    data += "\"Water_Temperature\": " + String(waterTempSensor.readValue()) + ", ";
    data += "\"Air_Temperature\": " + String(airTempSensor.readValue()) + ", ";
    data += "\"pH\": " + String(phSensor.readValue()) + ", ";
    data += "\"Turbidity\": " + String(turbiditySensor.readValue()) + ", ";
    data += "\"Dissolved_Oxygen\": " + String(oxygenSensor.readValue()) + ", ";
    data += "\"Air_Flow\": " + String(airFlowSensor.readValue()) + "}";

    espSerial.println(data); // Sends data to ESP via Serial
}
