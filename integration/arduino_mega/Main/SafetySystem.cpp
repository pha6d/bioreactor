#include "SafetySystem.h"
#include "Logger.h"

SafetySystem::SafetySystem(float totalVolume, float maxVolumePercent, float minVolume)
    : totalVolume(totalVolume), maxVolumePercent(maxVolumePercent), minVolume(minVolume), stopRequired(false), logger(nullptr) {
    // Initialization code here
}

void SafetySystem::checkLimits(float waterTemp, float airTemp, float pH, float dissolvedOxygen, float turbidity, float currentVolume) {
    stopRequired = false;
    checkWaterTemperature(waterTemp);
    checkAirTemperature(airTemp);
    checkPH(pH);
    checkDissolvedOxygen(dissolvedOxygen);
    checkVolume(currentVolume);
}

void SafetySystem::checkWaterTemperature(float temp) {
    if (temp < 31.0) logAlert("Water temperature low", AlertLevel::WARNING);
    if (temp > 41.0) logAlert("Water temperature high", AlertLevel::WARNING);
    if (temp > 46.0) {
        logAlert("Water temperature critical", AlertLevel::ALARM);
        stopRequired = true;
    }
}

void SafetySystem::checkAirTemperature(float temp) {
    if (temp < 15.0) logAlert("Air temperature low", AlertLevel::WARNING);
    if (temp > 35.0) logAlert("Air temperature high", AlertLevel::WARNING);
}

void SafetySystem::checkPH(float pH) {
    if (pH < 6.0) logAlert("pH low", AlertLevel::WARNING);
    if (pH > 7.5) logAlert("pH high", AlertLevel::WARNING);
    if (pH > 8.0) {
        logAlert("pH critical", AlertLevel::ALARM);
        stopRequired = true;
    }
}

void SafetySystem::checkDissolvedOxygen(float do_percent) {
    if (do_percent < 20.0) logAlert("Dissolved oxygen low", AlertLevel::WARNING);
}

void SafetySystem::checkVolume(float volume) {
    if (volume <= minVolume) {
        logAlert("Volume below minimum", AlertLevel::WARNING);
    }
    if (volume >= maxVolumePercent * totalVolume) {
        logAlert("Volume near maximum", AlertLevel::ALARM);
        stopRequired = true;
    }
}

void SafetySystem::logAlert(const String& message, AlertLevel level) {
    Logger::logAlert(message, level);
}