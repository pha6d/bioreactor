#include "StateMachine.h"

StateMachine::StateMachine(Logger& logger, PIDManager& pidManager, VolumeManager& volumeManager)
    : currentState(ProgramState::IDLE), currentProgram("None"), logger(logger),
      pidManager(pidManager), fermentationProgram(pidManager, volumeManager),
      testRunning(false), startTime(0) {}

void StateMachine::update(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                          PeristalticPump& basePump, StirringMotor& stirringMotor,
                          HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight) {
    // Update all running programs
    if (drainProgram.isRunning()) {
        drainProgram.update();
    }
    if (mixProgram.isRunning()) {
        mixProgram.update();
    }
    if (testProgram.isRunning()) {
        testProgram.update();
    }
    if (fermentationProgram.isRunning()) {
        fermentationProgram.update();
    }

    // Update state and program
    updateStateAndProgram();

    // Check if we need to stop any processes
    if (currentState == ProgramState::COMPLETED || currentState == ProgramState::ERROR) {
        stopAll(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight);
    }
}

void StateMachine::startDrain(DCPump& drainPump, int rate, int duration) {
    logger.logInfo("Starting drain process...");
    drainProgram.begin(drainPump, rate, duration);
    currentState = ProgramState::RUNNING;
    currentProgram = "Drain";
}

void StateMachine::startMix(StirringMotor& stirringMotor, int speed) {
    logger.logInfo("Starting mix process...");
    mixProgram.begin(stirringMotor, speed);
    currentState = ProgramState::RUNNING;
    currentProgram = "Mix";
}

void StateMachine::startTests(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor, 
                              PeristalticPump& nutrientPump, PeristalticPump& basePump, 
                              HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                              PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                              PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
                              OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor) {
    logger.logInfo("Starting tests...");
    testProgram.begin(airPump, drainPump, stirringMotor, nutrientPump, basePump, heatingPlate, ledGrowLight,
                      waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
    currentState = ProgramState::RUNNING;
    currentProgram = "Tests";
    testRunning = true;
}

void StateMachine::startFermentation(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump, 
                                     PeristalticPump& basePump, StirringMotor& stirringMotor, 
                                     HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                                     PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                                     PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
                                     OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
                                     float tempSetpoint, float phSetpoint, float doSetpoint,
                                     float nutrientConc, float baseConc, int duration,
                                     const String& experimentName, const String& comment) {
    logger.logInfo("Starting fermentation process...");
    fermentationProgram.begin(airPump, drainPump, nutrientPump, basePump, stirringMotor, heatingPlate, ledGrowLight,
                              waterTempSensor, airTempSensor, phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                              tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, experimentName, comment);
    currentState = ProgramState::RUNNING;
    currentProgram = "Fermentation";
}

void StateMachine::stopAll(DCPump& airPump, DCPump& drainPump, PeristalticPump& nutrientPump,
                           PeristalticPump& basePump, StirringMotor& stirringMotor,
                           HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight) {
    logger.logInfo("Stopping all processes...");
    airPump.control(false, 0);
    drainPump.control(false, 0);
    nutrientPump.control(false, 0);
    basePump.control(false, 0);
    stirringMotor.control(false, 0);
    heatingPlate.control(false);
    ledGrowLight.control(false);
    pidManager.stopTemperaturePID();
    pidManager.stopPHPID();
    pidManager.stopDOPID();
    stopAllTests();
    currentState = ProgramState::IDLE;
    currentProgram = "None";
}

String StateMachine::getCurrentProgram() const {
    return currentProgram;
}

String StateMachine::getCurrentStatus() const {
    switch (currentState) {
        case ProgramState::IDLE: return "Idle";
        case ProgramState::RUNNING: return "Running";
        case ProgramState::PAUSED: return "Paused";
        case ProgramState::COMPLETED: return "Completed";
        case ProgramState::ERROR: return "Error";
        default: return "Unknown";
    }
}

void StateMachine::stopAllTests() {
    testRunning = false;
    logger.logInfo("All tests stopped.");
}

bool StateMachine::isTestRunning() const {
    return testRunning;
}

void StateMachine::startTemperaturePID(double setpoint) {
    logger.logInfo("Starting Temperature PID control...");
    pidManager.startTemperaturePID(setpoint);
    currentState = ProgramState::RUNNING;
    currentProgram = "Temperature PID";
}

void StateMachine::startPHPID(double setpoint) {
    logger.logInfo("Starting pH PID control...");
    pidManager.startPHPID(setpoint);
    currentState = ProgramState::RUNNING;
    currentProgram = "pH PID";
}

void StateMachine::startDOPID(double setpoint) {
    logger.logInfo("Starting DO PID control...");
    pidManager.startDOPID(setpoint);
    currentState = ProgramState::RUNNING;
    currentProgram = "DO PID";
}

void StateMachine::stopTemperaturePID() {
    logger.logInfo("Stopping Temperature PID control...");
    pidManager.stopTemperaturePID();
    updateStateAndProgram();
}

void StateMachine::stopPHPID() {
    logger.logInfo("Stopping pH PID control...");
    pidManager.stopPHPID();
    updateStateAndProgram();
}

void StateMachine::stopDOPID() {
    logger.logInfo("Stopping DO PID control...");
    pidManager.stopDOPID();
    updateStateAndProgram();
}

void StateMachine::updateStateAndProgram() {
    if (drainProgram.isRunning()) {
        currentProgram = "Drain";
        currentState = ProgramState::RUNNING;
    } else if (mixProgram.isRunning()) {
        currentProgram = "Mix";
        currentState = ProgramState::RUNNING;
    } else if (testProgram.isRunning()) {
        currentProgram = "Tests";
        currentState = ProgramState::RUNNING;
    } else if (fermentationProgram.isRunning()) {
        currentProgram = "Fermentation";
        currentState = ProgramState::RUNNING;
    } else if (pidManager.isTestRunning()) {
        currentProgram = "PID Test";
        currentState = ProgramState::RUNNING;
    } else {
        currentProgram = "None";
        currentState = ProgramState::IDLE;
    }

    // Check if any program has just completed
    if (currentState == ProgramState::RUNNING && 
        !drainProgram.isRunning() && !mixProgram.isRunning() && 
        !testProgram.isRunning() && !fermentationProgram.isRunning() &&
        !pidManager.isTestRunning()) {
        currentState = ProgramState::COMPLETED;
        logger.logInfo("Program completed: " + currentProgram);
    }
}