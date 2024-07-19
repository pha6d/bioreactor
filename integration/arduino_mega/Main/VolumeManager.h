// VolumeManager.h
#ifndef VOLUME_MANAGER_H
#define VOLUME_MANAGER_H

#include "PeristalticPump.h"
#include "DCPump.h"
#include <Arduino.h>


class VolumeManager {
public:
    VolumeManager(float totalVolume, float maxVolumePercent, float minVolume,
                  PeristalticPump& nutrientPump, PeristalticPump& basePump, DCPump& drainPump);

    void updateVolume();
    void manuallyAdjustVolume(float volume, const String& source);
    float getCurrentVolume() const { return currentVolume; }
    void setInitialVolume(float volume);

private:
    PeristalticPump& nutrientPump;
    PeristalticPump& basePump;
    DCPump& drainPump;
    float totalVolume;
    float maxVolumePercent;
    float minVolume;
    float currentVolume;
    float addedNaOH;
    float addedNutrient;
    float addedMicroalgae;
    float removedVolume;
};

#endif // VOLUME_MANAGER_H