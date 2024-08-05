// StateMachine.cpp
#include "StateMachine.h"
#include <MemoryFree.h>

StateMachine::StateMachine(Logger& logger, PIDManager& pidManager, VolumeManager& volumeManager)
    : currentState(ProgramState::IDLE),
      currentProgram(nullptr),
      logger(logger),
      pidManager(pidManager),
      volumeManager(volumeManager)
{
}

void StateMachine::addProgram(const String& name, ProgramBase* program) {
    if (programs.getSize() < MAX_PROGRAMS) {
        programs.insert(name, program);
    } else {
        logger.log(LogLevel::ERROR, "Maximum number of programs reached");
    }
}

/*
void StateMachine::update() {
    if (currentProgram && currentState == ProgramState::RUNNING) {
        currentProgram->update();
        if (!currentProgram->isRunning()) {
            transitionToState(ProgramState::COMPLETED);
        }
    }
}
*/

void StateMachine::update() {
    if (currentProgram && currentState == ProgramState::RUNNING) {
        currentProgram->update();
        if (!currentProgram->isRunning()) {
            transitionToState(ProgramState::COMPLETED);
            currentProgram = nullptr; 
            Logger::log(LogLevel::INFO, "Program completed and cleared");
        }
    }
}


void StateMachine::startProgram(const String& programName, const String& command) {
    ProgramBase** program = programs.find(programName);
    if (program) {
        //stopProgram();
        currentProgram = *program;
        currentProgram->start(command);
        transitionToState(ProgramState::RUNNING);
        logger.log(LogLevel::INFO, "Started program: " + programName);
    } else {
        logger.log(LogLevel::WARNING, "Program not found: " + programName);
    }
}

/*
// This method stops the currently running program if it matches the given program name
void StateMachine::stopProgram(const String& programName) {
  // Check if there is a current program running and if its name matches the given program name
    if (currentProgram && currentProgram->getName() == programName) {
      // Stop the current program by calling its stop method
        currentProgram->stop();

        currentProgram = nullptr;
        // Transition the state machine to the STOPPED state
        transitionToState(ProgramState::STOPPED);
        // Log the action of stopping the program
        Logger::log(LogLevel::INFO, "Stopped program: " + programName);
    }
}
*/

// This method stops the currently running program if it matches the given program name
void StateMachine::stopProgram(const String& programName) {   //TEST
  // Check if there is a current program running and if its name matches the given program name
    if (currentProgram && currentProgram->getName() == programName) {
      // Stop the current program by calling its stop method
        currentProgram->stop();

        currentProgram = nullptr;
        // Transition the state machine to the STOPPED state
        transitionToState(ProgramState::STOPPED);
        // Log the action of stopping the program
        Logger::log(LogLevel::INFO, "Stopped program: " + programName);
    }
}


void StateMachine::stopAllPrograms() {
    if (currentProgram) {
        Logger::log(LogLevel::INFO, "Stopping current program: " + currentProgram->getName());
        currentProgram->stop();
        currentProgram = nullptr; 
        transitionToState(ProgramState::STOPPED);
        Logger::log(LogLevel::INFO, "All programs stopped");
        //Logger::log(LogLevel::INFO, "Current state: " + programStateToString(getCurrentState()));
        Logger::log(LogLevel::INFO, "Current state: " + String(static_cast<int>(getCurrentState())));
        Logger::log(LogLevel::INFO, "Current program: None");  // Modifiez cette ligne
    }
}


/*
void StateMachine::stopAllPrograms() {
    Logger::log(LogLevel::INFO, "Entering stopAllPrograms");
    if (currentProgram) {
        Logger::log(LogLevel::INFO, "Current program: " + currentProgram->getName());
        Logger::log(LogLevel::INFO, "About to stop current program");
        currentProgram->stop();
        Logger::log(LogLevel::INFO, "Current program stopped successfully");
        currentProgram = nullptr;
        Logger::log(LogLevel::INFO, "currentProgram set to nullptr");
        transitionToState(ProgramState::STOPPED);
        Logger::log(LogLevel::INFO, "Transitioned to STOPPED state");
    } else {
        Logger::log(LogLevel::INFO, "No current program to stop");
    }
    Logger::log(LogLevel::INFO, "About to stop all actuators");
    ActuatorController::stopAllActuators();
    Logger::log(LogLevel::INFO, "All actuators stopped");
    Logger::log(LogLevel::INFO, "Exiting stopAllPrograms");
}
*/


/*

    // Iterate through all registered programs and stop them
    Logger::log(LogLevel::INFO, "Iterating through all programs");
    print(freeMemory());
    for (int i = 0; i < programs.getSize(); i++) {
        // Get the program at the current index
        print(freeMemory());
        ProgramBase* program = programs.valueAt(i);  // Changed from getByIndex to valueAt
        print(freeMemory());
        if (program) {
            // Stop the program if it exists
            print(freeMemory());
            program->stop();

        }
    }
    // Reset the current program pointer
    print(freeMemory());
    currentProgram = nullptr;
    print(freeMemory());
    // Transition the state machine to the STOPPED state
    transitionToState(ProgramState::STOPPED);
    print(freeMemory());
    // Log that all programs have been stopped
    Logger::log(LogLevel::INFO, "All programs stopped");
    
}
*/





/*
void StateMachine::stopAllPrograms() {

    Logger::log(LogLevel::INFO, "Entering stopAllPrograms"); ///
    // Stop the currently running program, if any
    if (currentProgram) {
        Logger::log(LogLevel::INFO, "Stopping current program: " + currentProgram->getName()); ///
        currentProgram->stop();
    }



    // Iterate through all registered programs and stop them
    Logger::log(LogLevel::INFO, "Iterating through all programs");
    print(freeMemory());
    for (int i = 0; i < programs.getSize(); i++) {
        // Get the program at the current index
        print(freeMemory());
        ProgramBase* program = programs.valueAt(i);  // Changed from getByIndex to valueAt
        print(freeMemory());
        if (program) {
            // Stop the program if it exists
            print(freeMemory());
            program->stop();

        }
    }
    // Reset the current program pointer
    print(freeMemory());
    currentProgram = nullptr;
    print(freeMemory());
    // Transition the state machine to the STOPPED state
    transitionToState(ProgramState::STOPPED);
    print(freeMemory());
    // Log that all programs have been stopped
    Logger::log(LogLevel::INFO, "All programs stopped");
    
}
*/


/*
void StateMachine::stopAllPrograms() {

    Logger::log(LogLevel::INFO, "Iterating through all programs");
    Serial.print("Free memory: ");
    Serial.println(freeMemory());

    for (int i = 0; i < programs.getSize(); i++) {
        // Get the program at the current index
        Serial.print("Free memory before getting program: ");
        Serial.println(freeMemory());
        
        ProgramBase* program = programs.valueAt(i);  // Changed from getByIndex to valueAt
        Serial.print("Free memory after getting program: ");
        Serial.println(freeMemory());

        if (program) {
            // Stop the program if it exists
            Serial.print("Free memory before stopping program: ");
            Serial.println(freeMemory());
            
            program->stop();
            Serial.print("Free memory after stopping program: ");
            Serial.println(freeMemory());
        }
    }
    // Reset the current program pointer
    Serial.print("Free memory before resetting current program: ");
    Serial.println(freeMemory());
    
    currentProgram = nullptr;
    
    Serial.print("Free memory after resetting current program: ");
    Serial.println(freeMemory());

    // Transition the state machine to the STOPPED state
    transitionToState(ProgramState::STOPPED);
    
    Serial.print("Free memory after transitioning state: ");
    Serial.println(freeMemory());

    // Log that all programs have been stopped
    Logger::log(LogLevel::INFO, "All programs stopped");
}
*/


ProgramState StateMachine::getCurrentState() const {
    return currentState;
}

String StateMachine::getCurrentProgram() const {
    return currentProgram ? currentProgram->getName() : "None";
}

void StateMachine::transitionToState(ProgramState newState) {
    if (newState != currentState) {
        currentState = newState;
        Logger::log(LogLevel::INFO, "State changed to: " + String(static_cast<int>(currentState)));
    }
}

