#include <Arduino.h>
#include <SoftwareSerial.h>
#include "StateMachine.h"
#include "Logger.h" // Inclure Logger

// Inclure les interfaces
#include "ActuatorInterface.h"
#include "SensorInterface.h"

// Inclure les actionneurs
#include "PeristalticPump.h"
#include "DCPump.h"
#include "StirringMotor.h"
#include "HeatingPlate.h"
#include "LEDGrowLight.h"

// Inclure les capteurs
#include "PHSensor.h"
#include "OxygenSensor.h"
#include "TurbiditySensor.h"
#include "DS18B20TemperatureSensor.h"
#include "PT100Sensor.h"
#include "AirFlowSensor.h"

// Inclure les programmes
#include "TestActuatorsAndSensors.h"
#include "Drain.h"
#include "Stop.h"
#include "Mix.h"
#include "Fermentation.h"

// Définir le port série pour la communication avec l'ESP32
#define SerialESP Serial1

// Créer des objets avec des affectations de broches spécifiques et des valeurs si applicable
// Actionneurs
DCPump airPump(5, 6, 10, "Air Pump");
DCPump drainPump(3, 4, 15, "Drain Pump");
PeristalticPump nutrientPump(0x61, 7, 105.0, "Nutrient Pump");
PeristalticPump basePump(0x60, 8, 105.0, "Base Pump");
StirringMotor stirringMotor(9, 10);
HeatingPlate heatingPlate(13, "Heating Plate");
LEDGrowLight ledGrowLight(27, "LED Grow Light");

// Capteurs
PT100Sensor waterTempSensor(22, 23, 24, 25);
DS18B20TemperatureSensor airTempSensor(52);
PHSensor phSensor(A1);
TurbiditySensor turbiditySensor(A2);
OxygenSensor oxygenSensor(A3, &waterTempSensor);
AirFlowSensor airFlowSensor(26);

// Déclarer stopFlag comme variable globale
bool stopFlag = false;
String currentProgram = "None";
String programStatus = "Idle";

// Instancier le programme de drainage, de mélange et la machine à états
DrainProgram drainProgram;
MixProgram mixProgram;
StateMachine stateMachine;

// Variables pour le timing du logger
unsigned long previousMillis = 0;
const long interval = 1000; // Intervalle pour la journalisation (1 seconde)

void setup() {
    Serial.begin(115200);
    Serial1.begin(9600);

    Serial.println("Setup started");

    // Initialiser les capteurs et les actionneurs
    phSensor.begin();
    turbiditySensor.begin();
    oxygenSensor.begin();
    airTempSensor.begin();
    waterTempSensor.begin();
    airFlowSensor.begin();
    nutrientPump.begin();
    basePump.begin();
    heatingPlate.control(false);
    ledGrowLight.control(false);

    Serial.println("Setup completed");
    Serial.println();
}

void loop() {
    if (Serial.available() > 0) {
        String command = Serial.readStringUntil('\n');
        command.trim();

        Serial.print("Received command: ");
        Serial.println(command);

        if (command.equalsIgnoreCase("tests")) {
            Serial.println("Starting tests...");
            stateMachine.startTests(airPump, drainPump, stirringMotor, nutrientPump, basePump, heatingPlate, ledGrowLight,
                                    waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
        } else if (command.startsWith("drain")) {
            Serial.println("Starting drain...");
            int spaceIndex1 = command.indexOf(' ');
            int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
            int rate = command.substring(spaceIndex1 + 1, spaceIndex2).toInt();
            int duration = command.substring(spaceIndex2 + 1).toInt();
            stateMachine.startDrain(drainPump, rate, duration);
        } else if (command.equalsIgnoreCase("stop")) {
            Serial.println("Stopping all...");
            stateMachine.stopAll(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
        } else if (command.startsWith("mix")) {
            Serial.println("Starting mix...");
            int spaceIndex = command.indexOf(' ');
            int speed = command.substring(spaceIndex + 1).toInt();
            stateMachine.startMix(stirringMotor, speed);
        } else if (command.startsWith("fermentation")) {
            Serial.println("Starting fermentation...");
            int spaceIndex1 = command.indexOf(' ');
            int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
            int spaceIndex3 = command.indexOf(' ', spaceIndex2 + 1);
            int spaceIndex4 = command.indexOf(' ', spaceIndex3 + 1);
            int spaceIndex5 = command.indexOf(' ', spaceIndex4 + 1);
            int spaceIndex6 = command.indexOf(' ', spaceIndex5 + 1);
            int spaceIndex7 = command.indexOf(' ', spaceIndex6 + 1);
            int spaceIndex8 = command.indexOf(' ', spaceIndex7 + 1);

            float tempSetpoint = command.substring(spaceIndex1 + 1, spaceIndex2).toFloat();
            float phSetpoint = command.substring(spaceIndex2 + 1, spaceIndex3).toFloat();
            float doSetpoint = command.substring(spaceIndex3 + 1, spaceIndex4).toFloat();
            float nutrientConc = command.substring(spaceIndex4 + 1, spaceIndex5).toFloat();
            float baseConc = command.substring(spaceIndex5 + 1, spaceIndex6).toFloat();
            int duration = command.substring(spaceIndex6 + 1, spaceIndex7).toInt();
            String experimentName = command.substring(spaceIndex7 + 1, spaceIndex8);
            String comment = command.substring(spaceIndex8 + 1);

            stateMachine.startFermentation(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                                           waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                                           tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, experimentName, comment);
        }
    }

    // Journaliser les données toutes les secondes
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        logData(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                currentProgram, programStatus);
    }

    stateMachine.update(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
}
