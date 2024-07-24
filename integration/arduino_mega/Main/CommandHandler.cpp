#include "CommandHandler.h"

CommandHandler::CommandHandler(StateMachine& stateMachine, SafetySystem& safetySystem, 
                               VolumeManager& volumeManager, Logger& logger,
                               DCPump& airPump, DCPump& drainPump, 
                               PeristalticPump& nutrientPump, PeristalticPump& basePump,
                               StirringMotor& stirringMotor, HeatingPlate& heatingPlate, 
                               LEDGrowLight& ledGrowLight, PT100Sensor& waterTempSensor, 
                               DS18B20TemperatureSensor& airTempSensor, PHSensor& phSensor, 
                               TurbiditySensor& turbiditySensor, OxygenSensor& oxygenSensor, 
                               AirFlowSensor& airFlowSensor, PIDManager& pidManager)
    : stateMachine(stateMachine), safetySystem(safetySystem), 
      volumeManager(volumeManager), logger(logger),
      airPump(airPump), drainPump(drainPump), 
      nutrientPump(nutrientPump), basePump(basePump),
      stirringMotor(stirringMotor), heatingPlate(heatingPlate), 
      ledGrowLight(ledGrowLight), waterTempSensor(waterTempSensor), 
      airTempSensor(airTempSensor), phSensor(phSensor), 
      turbiditySensor(turbiditySensor), oxygenSensor(oxygenSensor), 
      airFlowSensor(airFlowSensor), pidManager(pidManager) {}

void CommandHandler::executeCommand(const String& command) {
    logger.logInfo("Executing command: " + command);

    if (command.equalsIgnoreCase("help")) {
        printHelp();
    } else if (command.startsWith("test ")) {
        String testCommand = command.substring(5); 
        // Implement test command logic here
    } else if (command.equalsIgnoreCase("tests")) {
        logger.logInfo("Starting tests...");
        stateMachine.startTests(airPump, drainPump, stirringMotor, nutrientPump, basePump, 
                                heatingPlate, ledGrowLight, waterTempSensor, airTempSensor,
                                phSensor, turbiditySensor, oxygenSensor, airFlowSensor);
    } else if (command.startsWith("drain")) {
        int rate, duration;
        sscanf(command.c_str(), "drain %d %d", &rate, &duration);
        stateMachine.startDrain(drainPump, rate, duration);
    } else if (command.equalsIgnoreCase("stop")) {
        logger.logInfo("Stopping all...");
        stateMachine.stopAllPrograms();
    } else if (command.startsWith("mix")) {
        int speed;
        sscanf(command.c_str(), "mix %d", &speed);
        stateMachine.startMix(stirringMotor, speed);
    } else if (command.startsWith("fermentation")) {
        float tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc;
        int duration;
        String experimentName, comment;
        parseFermentationParams(command, tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc, duration, experimentName, comment);
        stateMachine.startFermentation(airPump, drainPump, nutrientPump, basePump, stirringMotor,
                                       heatingPlate, ledGrowLight, waterTempSensor, airTempSensor,
                                       phSensor, turbiditySensor, oxygenSensor, airFlowSensor,
                                       tempSetpoint, phSetpoint, doSetpoint, nutrientConc, baseConc,
                                       duration, experimentName, comment);
    } else if (command.startsWith("pid ")) {
        String pidType = command.substring(4, 8);  // Extract PID type (temp, ph, or do)
        float setpoint = command.substring(9).toFloat();  // Extract setpoint
        stateMachine.startPIDTest(pidType, setpoint);
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
    } else {
        logger.logWarning("Unknown command: " + command);
    }
}

void CommandHandler::printHelp() {
    Serial.println();
    Serial.println("------------------------------------------------- Available commands: -------------------------------------------------");
    Serial.println("help - Display this help message");
    Serial.println("test <actuator> <value> <duration> - Test a specific actuator");
    Serial.println("  Available actuators:");
    Serial.print("    basePump <flow_rate_0_");
    Serial.print(basePump.getMaxFlowRate(), 1);  // 1 decimal place
    Serial.println("_ml_per_min> <duration_seconds>");
    Serial.print("    nutrientPump <flow_rate_0_");
    Serial.print(nutrientPump.getMaxFlowRate(), 1);  // 1 decimal place
    Serial.println("_ml_per_min> <duration_seconds>");
    Serial.println("    airPump <speed_0_100%> <duration_seconds>");
    Serial.println("    drainPump <speed_0_100%> <duration_seconds>");
    Serial.print("    stirringMotor <speed_");
    Serial.print(stirringMotor.getMinRPM());
    Serial.print("_");
    Serial.print(stirringMotor.getMaxRPM());
    Serial.println("> <duration_seconds>");
    Serial.println("    heatingPlate <power_0_100%> <duration_seconds>");
    Serial.println("    ledGrowLight <intensity_0_100%> <duration_seconds>");
    Serial.println("tests - Run all predefined tests");
    Serial.println("drain <rate> <duration> - Start draining");
    Serial.println("stop - Stop all actuators and PIDs");
    Serial.println("mix <speed> - Start mixing");
    Serial.println("fermentation <temp> <ph> <do> <nutrient_conc> <base_conc> <duration> <experiment_name> <comment> - Start fermentation");
    Serial.println("pid <type> <setpoint> - Start PID control (type: temp, ph, or do)");
    Serial.println("adjust_volume <source> <amount> - Manually adjust volume");
    Serial.println("disable_alarms - Disable safety alarms");
    Serial.println("enable_alarms - Enable safety alarms");
    Serial.println("disable_warnings - Disable safety warnings");
    Serial.println("enable_warnings - Enable safety warnings");
    Serial.println("set_check_interval <seconds> - Set safety check interval");
    Serial.println("set_initial_volume <volume> - Set the initial culture volume (in liters)");
    Serial.println("-----------------------------------------------------------------------------------------------------------------------");
}

void CommandHandler::parseFermentationParams(const String& command, float& tempSetpoint, float& phSetpoint, 
                                             float& doSetpoint, float& nutrientConc, float& baseConc, 
                                             int& duration, String& experimentName, String& comment) {
    char expName[50], comm[50];
    sscanf(command.c_str(), "fermentation %f %f %f %f %f %d %s %s", 
           &tempSetpoint, &phSetpoint, &doSetpoint, &nutrientConc, &baseConc, &duration, 
           expName, comm);
    experimentName = String(expName);
    comment = String(comm);
}