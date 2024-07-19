#ifndef SAFETY_SYSTEM_H
#define SAFETY_SYSTEM_H

#include <Arduino.h>

// Forward declaration
class Logger;

enum class AlertLevel {
    WARNING,
    ALARM
};

class SafetySystem {
public:
    SafetySystem(float totalVolume, float maxVolumePercent, float minVolume);
    void checkLimits(float waterTemp, float airTemp, float pH, float dissolvedOxygen, float turbidity, float currentVolume);
    bool shouldStop() const { return stopRequired; }
    void setLogger(Logger* logger) { this->logger = logger; }
    void setAlarmEnabled(bool enabled) { alarmEnabled = enabled; }
    void setWarningEnabled(bool enabled) { warningEnabled = enabled; }
    void setCheckInterval(unsigned long interval) { checkInterval = interval; }

private:
    void checkWaterTemperature(float temp);
    void checkAirTemperature(float temp);
    void checkPH(float pH);
    void checkDissolvedOxygen(float do_percent);
    void checkVolume(float volume);
    void checkTurbidity(float turbidity);

    void logAlert(const String& message, AlertLevel level);

    float totalVolume;
    float maxVolumePercent;
    float minVolume;
    bool stopRequired;
    Logger* logger;

    bool alarmEnabled;
    bool warningEnabled;
    unsigned long lastCheckTime;
    unsigned long checkInterval;

    // Safety thresholds
    static constexpr float MIN_WATER_TEMP = 15.0;
    static constexpr float MAX_WATER_TEMP = 45.0;
    static constexpr float CRITICAL_WATER_TEMP = 50.0;
    static constexpr float MIN_AIR_TEMP = 10.0;
    static constexpr float MAX_AIR_TEMP = 40.0;
    static constexpr float MIN_PH = 5.0;
    static constexpr float MAX_PH = 9.0;
    static constexpr float CRITICAL_PH = 10.0;
    static constexpr float MIN_DO = 20.0;
    static constexpr float MAX_TURBIDITY = 1000.0; // Example value, adjust as needed
};

#endif // SAFETY_SYSTEM_H