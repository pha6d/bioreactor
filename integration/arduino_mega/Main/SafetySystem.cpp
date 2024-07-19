#include "SafetySystem.h"
#include "Logger.h"

SafetySystem::SafetySystem(float totalVolume, float maxVolumePercent, float minVolume)
    : totalVolume(totalVolume), maxVolumePercent(maxVolumePercent), minVolume(minVolume), 
      stopRequired(false), logger(nullptr), alarmEnabled(true), warningEnabled(true),
      lastCheckTime(0), checkInterval(30000) {} // 30 seconds by default

void SafetySystem::checkLimits(float waterTemp, float airTemp, float pH, float dissolvedOxygen, float turbidity, float currentVolume) {
    unsigned long currentTime = millis();
    if (currentTime - lastCheckTime < checkInterval) {
        return; // Do not check if the interval has not elapsed
    }
    lastCheckTime = currentTime;

    stopRequired = false;
    checkWaterTemperature(waterTemp);
    checkAirTemperature(airTemp);
    checkPH(pH);
    checkDissolvedOxygen(dissolvedOxygen);
    checkVolume(currentVolume);
    checkTurbidity(turbidity);
}

void SafetySystem::checkWaterTemperature(float temp) {
    if (temp < MIN_WATER_TEMP) logAlert("Water temperature low", AlertLevel::WARNING);
    if (temp > MAX_WATER_TEMP) logAlert("Water temperature high", AlertLevel::WARNING);
    if (temp > CRITICAL_WATER_TEMP) {
        logAlert("Water temperature critical", AlertLevel::ALARM);
        stopRequired = true;
    }
}

void SafetySystem::checkAirTemperature(float temp) {
    if (temp < MIN_AIR_TEMP) logAlert("Air temperature low", AlertLevel::WARNING);
    if (temp > MAX_AIR_TEMP) logAlert("Air temperature high", AlertLevel::WARNING);
}

void SafetySystem::checkPH(float pH) {
    if (pH < MIN_PH) logAlert("pH low", AlertLevel::WARNING);
    if (pH > MAX_PH) logAlert("pH high", AlertLevel::WARNING);
    if (pH > CRITICAL_PH) {
        logAlert("pH critical", AlertLevel::ALARM);
        stopRequired = true;
    }
}

void SafetySystem::checkDissolvedOxygen(float do_percent) {
    if (do_percent < MIN_DO) logAlert("Dissolved oxygen low", AlertLevel::WARNING);
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

void SafetySystem::checkTurbidity(float turbidity) {
    if (turbidity > MAX_TURBIDITY) {
        logAlert("Turbidity high", AlertLevel::WARNING);
    }
}

void SafetySystem::logAlert(const String& message, AlertLevel level) {
    if (logger) {
        if ((level == AlertLevel::ALARM && alarmEnabled) || 
            (level == AlertLevel::WARNING && warningEnabled)) {
            logger->logAlert(message, level);
        }
    }
}