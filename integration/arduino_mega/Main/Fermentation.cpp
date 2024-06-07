#include "Fermentation.h"
#include "PIDControllers.h"
#include "Logger.h"
#include <SoftwareSerial.h>


// Include extern declarations
extern bool stopFlag;
extern void runStop(DCPump&, DCPump&, PeristalticPump&, PeristalticPump&, StirringMotor&, HeatingPlate&, LEDGrowLight&);

extern SoftwareSerial espSerial;

void runFermentation(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, PeristalticPump& basePump, StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
    PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
    float tempSetpointVal, float phSetpointVal, float doSetpointVal, float nutrientConc, float baseConc, int duration, const String& experimentName, const String& comment) {
    // Set the setpoints
    tempSetpoint = tempSetpointVal;
    phSetpoint = phSetpointVal;
    doSetpoint = doSetpointVal;

    tempPID.SetMode(AUTOMATIC);
    phPID.SetMode(AUTOMATIC);
    doPID.SetMode(AUTOMATIC);

    Serial.println(), Serial.println("run FERMENTATION program");
    unsigned long startTime = millis();
    while ((millis() - startTime) < (duration * 1000)) {
        if (stopFlag) {
            break; // Exit the loop if stop is requested
        }

        // Read sensors
        float waterTemp = waterTempSensor.readValue();
        float ph = phSensor.readValue();
        float doValue = oxygenSensor.readValue();

        // Update PID inputs
        tempInput = waterTemp;
        phInput = ph;
        doInput = doValue;

        // Compute PID outputs
        tempPID.Compute();
        phPID.Compute();
        doPID.Compute();

        // Control actuators
        heatingPlate.control(tempOutput > 0, tempOutput);
        nutrientPump.control(phOutput > 0, nutrientConc * phOutput / 100.0);
        basePump.control(phOutput <= 0, baseConc * (-phOutput) / 100.0);
        airPump.control(doOutput > 0, doOutput);
        stirringMotor.control(doOutput > 0, doOutput * 2); // Example of stirring speed adjustment based on DO control

        // Log data
        logData(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight, waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor, experimentName, comment, "Fermentation");

        delay(1000); // Delay for 1 second
    }

    // Ensure everything is stopped after fermentation
    runStop(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
}
