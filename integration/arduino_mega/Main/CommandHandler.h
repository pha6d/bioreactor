// CommandHandler.h

#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include "StateMachine.h"
#include "SafetySystem.h"
#include "VolumeManager.h"
#include "Logger.h"
#include "ActuatorController.h"

class CommandHandler {
public:
    CommandHandler(StateMachine& stateMachine, SafetySystem& safetySystem, 
                   VolumeManager& volumeManager, Logger& logger);

    // Execute a command received from the user
    void executeCommand(const String& command);

    // Print help information
    void printHelp();

private:
    StateMachine& stateMachine;
    SafetySystem& safetySystem;
    VolumeManager& volumeManager;
    Logger& logger;

    // Helper method to parse fermentation parameters
    void parseFermentationParams(const String& command, float& tempSetpoint, float& phSetpoint, 
                                 float& doSetpoint, float& nutrientConc, float& baseConc, 
                                 int& duration, String& experimentName, String& comment);
};

#endif // COMMAND_HANDLER_H