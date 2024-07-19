// CommandHandler.cpp

#include "CommandHandler.h"

CommandHandler::CommandHandler(StateMachine& stateMachine, SafetySystem& safetySystem, 
                               VolumeManager& volumeManager, Logger& logger)
    : stateMachine(stateMachine), safetySystem(safetySystem), 
      volumeManager(volumeManager), logger(logger) {}

void CommandHandler::executeCommand(const String& command) {
    logger.logInfo("Executing command: " + command);

    if (command.equalsIgnoreCase("help")) {
        printHelp();
    } else if (command.startsWith("test ")) {
        String testCommand = command.substring(5);  // Remove "test " from the beginning
        ActuatorController::executeActuator(testCommand);
    } else if (command.equalsIgnoreCase("tests")) {
        logger.logInfo("Starting tests...");
        // Implement the test start logic here
    } else if (command.startsWith("drain")) {
        logger.logInfo("Starting drain...");
        int spaceIndex1 = command.indexOf(' ');
        int spaceIndex2 = command.indexOf(' ', spaceIndex1 + 1);
        int rate = command.substring(spaceIndex1 + 1, spaceIndex2).toInt();
        int duration = command.substring(spaceIndex2 + 1).toInt();
        // Implement drain start logic here
    } else if (command.equalsIgnoreCase("stop")) {
        logger.logInfo("Stopping all...");
        // Implement stop all logic here
    } else if (command.startsWith("mix")) {
        logger.logInfo("Starting mix...");
        int spaceIndex = command.indexOf(' ');
        int speed = command.substring(spaceIndex + 1).toInt();
        // Implement mix start logic here
    } else if (command.startsWith("fermentation")) {
        logger.logInfo("Starting fermentation...");
        float tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc;
        int duration;
        String experimentName, comment;
        parseFermentationParams(command, tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, experimentName, comment);
        // Implement fermentation start logic here
    } else if (command.startsWith("pid ")) {
        String pidType = command.substring(4, 8);  // Extract PID type (temp, ph, or do)
        float setpoint = command.substring(9).toFloat();  // Extract setpoint
        
        if (pidType == "temp") {
            // Implement temperature PID start logic
        } else if (pidType == "ph") {
            // Implement pH PID start logic
        } else if (pidType == "do") {
            // Implement DO PID start logic
        } else {
            logger.logWarning("Unknown PID type: " + pidType);
        }
    } else if (command.startsWith("adjust_volume")) {
        int firstSpace = command.indexOf(' ');
        int secondSpace = command.indexOf(' ', firstSpace + 1);
        if (firstSpace != -1 && secondSpace != -1) {
            String source = command.substring(firstSpace + 1, secondSpace);
            float amount = command.substring(secondSpace + 1).toFloat();
            volumeManager.manuallyAdjustVolume(amount, source);
            logger.logInfo("Manual volume adjustment: " + source + " " + String(amount));
        }
    } else if (command == "disable_alarms") {
        safetySystem.setAlarmEnabled(false);
        logger.logInfo("Alarms disabled");
    } else if (command == "enable_alarms") {
        safetySystem.setAlarmEnabled(true);
        logger.logInfo("Alarms enabled");
    } else if (command == "disable_warnings") {
        safetySystem.setWarningEnabled(false);
        logger.logInfo("Warnings disabled");
    } else if (command == "enable_warnings") {
        safetySystem.setWarningEnabled(true);
        logger.logInfo("Warnings enabled");
    } else if (command.startsWith("set_check_interval")) {
        int interval = command.substring(19).toInt();
        safetySystem.setCheckInterval(interval * 1000); // Convert to milliseconds
        logger.logInfo("Safety check interval set to " + String(interval) + " seconds");
    } else if (command.startsWith("set_initial_volume")) {
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex != -1) {
          float initialVolume = command.substring(spaceIndex + 1).toFloat();
          volumeManager.setInitialVolume(initialVolume);
          logger.logInfo("Initial culture volume set to " + String(initialVolume) + " L");
      } else {
          logger.logWarning("Invalid set_initial_volume command. Usage: set_initial_volume <volume_in_liters>");
      }
  }
}

void CommandHandler::printHelp() {
    Serial.println("Available commands:");
    Serial.println("help - Display this help message");
    Serial.println("test <actuator> <value> <duration> - Test a specific actuator");
    Serial.println("  Available actuators:");
    Serial.println("    basePump <flow_rate_ml_per_min> <duration_seconds>");
    Serial.println("    nutrientPump <flow_rate_ml_per_min> <duration_seconds>");
    Serial.println("    airPump <speed_0_255> <duration_seconds>");
    Serial.println("    drainPump <speed_0_255> <duration_seconds>");
    Serial.println("    stirringMotor <speed_0_255> <duration_seconds>");
    Serial.println("    heatingPlate <temperature_celsius> <duration_seconds>");
    Serial.println("    ledGrowLight <intensity_0_255> <duration_seconds>");
    Serial.println("tests - Run all predefined tests");
    Serial.println("drain <rate> <duration> - Start draining");
    Serial.println("stop - Stop all actuators and PIDs");
    Serial.println("mix <speed> - Start mixing");
    Serial.println("fermentation <temp> <ph> <do> <nutrient_conc> <base_conc> <duration> <experiment_name> <comment> - Start fermentation");
    Serial.println("pid temp <setpoint> - Start temperature PID control");
    Serial.println("pid ph <setpoint> - Start pH PID control");
    Serial.println("pid do <setpoint> - Start dissolved oxygen PID control");
    Serial.println("adjust_volume <source> <amount> - Manually adjust volume (e.g., adjust_volume NaOH 10.5)");
    Serial.println("disable_alarms - Disable safety alarms");
    Serial.println("enable_alarms - Enable safety alarms");
    Serial.println("disable_warnings - Disable safety warnings");
    Serial.println("enable_warnings - Enable safety warnings");
    Serial.println("set_check_interval <seconds> - Set safety check interval");
    Serial.println("set_initial_volume <volume> - Set the initial culture volume (in liters)");
}

void CommandHandler::parseFermentationParams(const String& command, float& tempSetpoint, float& phSetpoint, 
                                             float& doSetpoint, float& nutrientConc, float& baseConc, 
                                             int& duration, String& experimentName, String& comment) {
    sscanf(command.c_str(), "fermentation %f %f %f %f %f %d %s %s", 
           &tempSetpoint, &phSetpoint, &doSetpoint, &nutrientConc, &baseConc, &duration, 
           experimentName.c_str(), comment.c_str());
}