#include "Logger.h"
#include <SoftwareSerial.h> // Include SoftwareSerial

extern SoftwareSerial espSerial; // Ensure the extern declaration is here

void logData(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump,
    StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor,
    TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
    const String& currentProgram, const String& programStatus) {

    String data = "{";
    data += "\"currentProgram\":\"" + currentProgram + "\",";
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

void logStartupParameters(const String& programType, int rateOrSpeed, int duration,
    float tempSetpoint, float phSetpoint, float doSetpoint, float nutrientConc,
    float baseConc, const String& experimentName, const String& comment) {

    String data = "{";
    data += "\"event\":\"startup\",";
    data += "\"programType\":\"" + programType + "\",";
    data += "\"rateOrSpeed\":" + String(rateOrSpeed) + ",";
    data += "\"duration\":" + String(duration) + ",";
    data += "\"tempSetpoint\":" + String(tempSetpoint) + ",";
    data += "\"phSetpoint\":" + String(phSetpoint) + ",";
    data += "\"doSetpoint\":" + String(doSetpoint) + ",";
    data += "\"nutrientConc\":" + String(nutrientConc) + ",";
    data += "\"baseConc\":" + String(baseConc) + ",";
    data += "\"experimentName\":\"" + experimentName + "\",";
    data += "\"comment\":\"" + comment + "\"";
    data += "}";

    espSerial.println(data);
    Serial.println(data); // Also print the data to the Serial monitor for debugging
}
