// VolumeManager.cpp
#include "VolumeManager.h"

VolumeManager::VolumeManager(float totalVolume, float maxVolumePercent, float minVolume,
                             PeristalticPump& nutrientPump, PeristalticPump& basePump, DCPump& drainPump)
    : totalVolume(totalVolume), maxVolumePercent(maxVolumePercent), minVolume(minVolume),
      currentVolume(minVolume), nutrientPump(nutrientPump), basePump(basePump), drainPump(drainPump),
      addedMicroalgae(0) {}

void VolumeManager::updateVolume() {
    currentVolume += (addedNaOH + addedNutrient + addedMicroalgae - removedVolume);
    currentVolume = max(minVolume, min(currentVolume, totalVolume));

    addedNaOH = 0;
    addedNutrient = 0;
    addedMicroalgae = 0;
    removedVolume = 0;
}

void VolumeManager::manuallyAdjustVolume(float volume, const String& source) {
    if (source == "NaOH") {
        addedNaOH += volume;
    } else if (source == "Nutrient") {
        addedNutrient += volume;
    } else if (source == "Microalgae") {
        addedMicroalgae += volume;
    } else if (source == "Removed") {
        removedVolume += volume;
    }
    updateVolume();
}

void VolumeManager::setInitialVolume(float volume) {
    if (volume > 0 && volume <= totalVolume) {
        currentVolume = volume;
        // Réinitialiser les autres volumes si nécessaire
        addedNaOH = 0;
        addedNutrient = 0;
        addedMicroalgae = 0;
        removedVolume = 0;
    }
}