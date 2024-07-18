/*
 * BioReactorManager.cpp
 * This file implements the BioReactorManager class defined in BioReactorManager.h.
 * It manages the overall operation of the bioreactor, coordinating between different components.
 */

#include "BioReactorManager.h"

BioReactorManager::BioReactorManager(FermentationProgram& fermentationProgram, 
                                     SafetySystem& safetySystem,
                                     Logger& logger,
                                     PIDManager& pidManager,
                                     DCPump& airPump,
                                     DCPump& drainPump,
                                     PeristalticPump& nutrientPump,
                                     PeristalticPump& basePump,
                                     StirringMotor& stirringMotor,
                                     HeatingPlate& heatingPlate,
                                     LEDGrowLight& ledGrowLight,
                                     PT100Sensor& waterTempSensor,
                                     DS18B20TemperatureSensor& airTempSensor,
                                     PHSensor& phSensor,
                                     TurbiditySensor& turbiditySensor,
                                     OxygenSensor& oxygenSensor,
                                     AirFlowSensor& airFlowSensor)
    : fermentationProgram(fermentationProgram), 
      safetySystem(safetySystem),
      logger(logger),
      pidManager(pidManager),
      airPump(airPump),
      drainPump(drainPump),
      nutrientPump(nutrientPump),
      basePump(basePump),
      stirringMotor(stirringMotor),
      heatingPlate(heatingPlate),
      ledGrowLight(ledGrowLight),
      waterTempSensor(waterTempSensor),
      airTempSensor(airTempSensor),
      phSensor(phSensor),
      turbiditySensor(turbiditySensor),
      oxygenSensor(oxygenSensor),
      airFlowSensor(airFlowSensor),
      currentVolume(0),
      totalVolume(0),
      maxVolumePercent(0),
      minVolume(0),
      addedNaOH(0),
      addedNutrient(0),
      addedMicroalgae(0),
      removedVolume(0)
{}

void BioReactorManager::initialize() {
    Serial.println("Initializing BioReactorManager...");
}

void BioReactorManager::update() {
    if (fermentationProgram.isRunning()) {
        fermentationProgram.update();
        checkSafetyLimits();
        logData();
        adjustStirringSpeed();
        updateVolume();
    }
}

void BioReactorManager::startFermentation(float tempSetpoint, float phSetpoint, float doSetpoint,
                                          float nutrientConc, float baseConc, int duration,
                                          const String& experimentName, const String& comment,
                                          float totalVolume, float maxVolumePercent, float minVolume) {
    this->totalVolume = totalVolume;
    this->maxVolumePercent = maxVolumePercent;
    this->minVolume = minVolume;
    this->currentVolume = minVolume;

    safetySystem = SafetySystem(totalVolume, maxVolumePercent, minVolume);

    fermentationProgram.begin(airPump, drainPump, nutrientPump, basePump, 
                              stirringMotor, heatingPlate, ledGrowLight,
                              waterTempSensor, airTempSensor, phSensor, 
                              turbiditySensor, oxygenSensor, airFlowSensor,
                              tempSetpoint, phSetpoint, doSetpoint, 
                              nutrientConc, baseConc, duration, 
                              experimentName, comment);

    Serial.println("Fermentation started: " + experimentName);
    Serial.println("Comment: " + comment);
}

void BioReactorManager::stopFermentation() {
    fermentationProgram.stop();
    // Assuming StateMachine is accessible, otherwise you'll need to modify this
    // stateMachine.stopAll(airPump, drainPump, nutrientPump, basePump, 
    //                      stirringMotor, heatingPlate, ledGrowLight);
    Serial.println("Fermentation stopped");
}

void BioReactorManager::handleCommand(const String& command) {
    Serial.println("Received command: " + command);
}

void BioReactorManager::testTemperaturePID(float targetTemperature) {
    pidManager.startTest("temp");
    pidManager.setTemperatureSetpoint(targetTemperature);
}

void BioReactorManager::testPHPID(float targetPH) {
    pidManager.startTest("ph");
    pidManager.setPHSetpoint(targetPH);
}

void BioReactorManager::testDOPID(float targetDO) {
    pidManager.startTest("do");
    pidManager.setDOSetpoint(targetDO);
}

void BioReactorManager::checkSafetyLimits() {
    safetySystem.checkLimits(waterTempSensor.readValue(),
                             airTempSensor.readValue(),
                             phSensor.readValue(),
                             oxygenSensor.readValue(),
                             turbiditySensor.readValue(),
                             currentVolume);

    if (safetySystem.shouldStop()) {
        stopFermentation();
    }
}

void BioReactorManager::logData() {
    // Using the existing logger.logData method
    logger.logData(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                   waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                   "Fermentation", fermentationProgram.isRunning() ? "Running" : "Stopped");

    // Additional logging to Serial for debugging
    Serial.println("Current Volume: " + String(currentVolume) +
                   ", Added NaOH: " + String(addedNaOH) +
                   ", Added Nutrient: " + String(addedNutrient) +
                   ", Added Microalgae: " + String(addedMicroalgae) +
                   ", Removed Volume: " + String(removedVolume));
}

void BioReactorManager::adjustStirringSpeed() {
    double tempOutput = pidManager.getTemperatureOutput();
    double phOutput = pidManager.getPHOutput();
    double doOutput = pidManager.getDOOutput();

    int stirringSpeed = static_cast<int>(max(max(abs(tempOutput), abs(phOutput)), abs(doOutput)));
    stirringMotor.control(true, stirringSpeed);
}

void BioReactorManager::updateVolume() {
    addedNaOH += basePump.getVolumeAdded();
    addedNutrient += nutrientPump.getVolumeAdded();
    removedVolume += drainPump.getVolumeRemoved();
    
    currentVolume += (addedNaOH + addedNutrient + addedMicroalgae - removedVolume);
    
    currentVolume = max(minVolume, min(currentVolume, totalVolume));

    basePump.resetVolumeAdded();
    nutrientPump.resetVolumeAdded();
    drainPump.resetVolumeRemoved();
}

void BioReactorManager::manuallyAdjustVolume(float volume, const String& source) {
    if (source == "NaOH") {
        addedNaOH += volume;
    } else if (source == "Nutrient") {
        addedNutrient += volume;
    } else if (source == "Microalgae") {
        addedMicroalgae += volume;
    } else if (source == "Removed") {
        removedVolume += volume;
    }
    updateVolume();
    Serial.println("Volume manually adjusted: " + source + " " + String(volume));
}