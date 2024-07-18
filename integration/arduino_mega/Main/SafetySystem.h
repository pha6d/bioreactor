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

private:
    void checkWaterTemperature(float temp);
    void checkAirTemperature(float temp);
    void checkPH(float pH);
    void checkDissolvedOxygen(float do_percent);
    void checkVolume(float volume);

    void logAlert(const String& message, AlertLevel level);

    float totalVolume;
    float maxVolumePercent;
    float minVolume;
    bool stopRequired;
    Logger* logger;  // Pointer to Logger
};

#endif // SAFETY_SYSTEM_H