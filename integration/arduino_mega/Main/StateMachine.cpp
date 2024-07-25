#include "StateMachine.h"


StateMachine::StateMachine(Logger& logger, PIDManager& pidManager, VolumeManager& volumeManager)
    : programCount(0),
      currentState(ProgramState::IDLE),
      currentProgram(nullptr),
      logger(logger),
      pidManager(pidManager),
      volumeManager(volumeManager)
{
}

void StateMachine::addProgram(ProgramBase* program) {
    if (programCount < MAX_PROGRAMS) {
        programs[programCount++] = program;
    } else {
        logger.logError("Maximum number of programs reached");
    }
}

void StateMachine::update() {
    if (currentProgram && currentState == ProgramState::RUNNING) {
        currentProgram->update();
        if (!currentProgram->isRunning()) {
            transitionToState(ProgramState::COMPLETED);
        }
    }
}

void StateMachine::startProgram(const String& programName) {
    for (int i = 0; i < programCount; i++) {
        if (programs[i]->getName() == programName) {
            if (currentProgram) {
                stopProgram(currentProgram->getName());
            }
            currentProgram = programs[i];
            transitionToState(ProgramState::RUNNING);
            logger.logInfo("Started program: " + programName);
            return;
        }
    }
    logger.logWarning("Program not found: " + programName);
}

void StateMachine::stopProgram(const String& programName) {
    if (currentProgram && currentProgram->getName() == programName) {
        currentProgram->stop();
        transitionToState(ProgramState::STOPPED);
        logger.logInfo("Stopped program: " + programName);
    }
}

void StateMachine::stopAllPrograms() {
    for (int i = 0; i < programCount; i++) {
        programs[i]->stop();
    }
    currentProgram = nullptr;
    transitionToState(ProgramState::STOPPED);
    logger.logInfo("All programs stopped");
}

ProgramState StateMachine::getCurrentState() const {
    return currentState;
}

String StateMachine::getCurrentProgram() const {
    return currentProgram ? currentProgram->getName() : "None";
}

void StateMachine::transitionToState(ProgramState newState) {
    if (newState != currentState) {
        currentState = newState;
        logger.logInfo("State changed to: " + String(static_cast<int>(currentState)));
    }
}

void StateMachine::configureDrain(DCPump& drainPump, int rate, int duration) {
    if (currentProgram && currentProgram->getName() == "Drain") {
        DrainProgram* program = static_cast<DrainProgram*>(currentProgram);
        program->configure(drainPump, rate, duration);
    }
}

void StateMachine::configureMix(StirringMotor& stirringMotor, int speed) {
    if (currentProgram && currentProgram->getName() == "Mix") {
        MixProgram* program = static_cast<MixProgram*>(currentProgram);
        program->configure(stirringMotor, speed);
    }
}

void StateMachine::configureFermentation(DCPump& airPump, DCPump& drainPump,
                                         PeristalticPump& nutrientPump, PeristalticPump& basePump,
                                         StirringMotor& stirringMotor, HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                                         PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                                         PHSensor& phSensor, TurbiditySensor& turbiditySensor,
                                         OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor,
                                         float tempSetpoint, float phSetpoint, float doSetpoint,
                                         float nutrientConc, float baseConc, int duration,
                                         const String& experimentName, const String& comment) {
    if (currentProgram && currentProgram->getName() == "Fermentation") {
        FermentationProgram* program = static_cast<FermentationProgram*>(currentProgram);
        program->configure(airPump, drainPump, nutrientPump, basePump, stirringMotor,
                           heatingPlate, ledGrowLight, waterTempSensor, airTempSensor,
                           phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                           tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc,
                           duration, experimentName, comment);
    }
}

void StateMachine::configurePIDTest(const String& pidType, double setpoint) {
    if (currentProgram && currentProgram->getName() == "PIDTest") {
        PIDTestProgram* program = static_cast<PIDTestProgram*>(currentProgram);
        program->configure(pidType, setpoint);
    }
}

void StateMachine::startDrain(DCPump& drainPump, int rate, int duration) {
    startProgram("Drain");
    configureDrain(drainPump, rate, duration);
    if (currentProgram && currentProgram->getName() == "Drain") {
        currentProgram->begin();
    }
}

void StateMachine::startMix(StirringMotor& stirringMotor, int speed) {
    startProgram("Mix");
    configureMix(stirringMotor, speed);
    if (currentProgram && currentProgram->getName() == "Mix") {
        currentProgram->begin();
    }
}

void StateMachine::startTests(DCPump& airPump, DCPump& drainPump, StirringMotor& stirringMotor, 
                              PeristalticPump& nutrientPump, PeristalticPump& basePump, 
                              HeatingPlate& heatingPlate, LEDGrowLight& ledGrowLight,
                              PT100Sensor& waterTempSensor, DS18B20TemperatureSensor& airTempSensor,
                              PHSensor& phSensor, TurbiditySensor& turbiditySensor, 
                              OxygenSensor& oxygenSensor, AirFlowSensor& airFlowSensor) {
    startProgram("TestActuators");
    if (currentProgram && currentProgram->getName() == "TestActuators") {
        TestActuatorsProgram* program = static_cast<TestActuatorsProgram*>(currentProgram);
        program->begin();
        program->configure(airPump, drainPump, stirringMotor, nutrientPump, basePump, 
                           heatingPlate, ledGrowLight, waterTempSensor, airTempSensor,
                           phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
    }
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
    startProgram("Fermentation");
    configureFermentation(airPump, drainPump, nutrientPump, basePump, stirringMotor,
                          heatingPlate, ledGrowLight, waterTempSensor, airTempSensor,
                          phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                          tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc,
                          duration, experimentName, comment);
    if (currentProgram && currentProgram->getName() == "Fermentation") {
        currentProgram->begin();
    }
}

void StateMachine::startPIDTest(const String& pidType, double setpoint) {
    startProgram("PIDTest");
    if (currentProgram && currentProgram->getName() == "PIDTest") {
        PIDTestProgram* program = static_cast<PIDTestProgram*>(currentProgram);
        program->begin();
        program->configure(pidType, setpoint);
    }
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

    stopAllPrograms();
}

void StateMachine::startActuatorTest(const String& actuatorName, float value, int duration) {
    startProgram("TestActuators");
    if (currentProgram && currentProgram->getName() == "TestActuators") {
        TestActuatorsProgram* program = static_cast<TestActuatorsProgram*>(currentProgram);
        program->begin();
        program->runIndividualTest(actuatorName, value, duration);
    }
}

void StateMachine::startSensorTest() {
    startProgram("TestActuators");
    if (currentProgram && currentProgram->getName() == "TestActuators") {
        TestActuatorsProgram* program = static_cast<TestActuatorsProgram*>(currentProgram);
        program->startSensorTest();
    }
}

