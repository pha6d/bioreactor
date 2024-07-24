/*
 * MixProgram.cpp
 * This file implements the MixProgram class defined in MixProgram.h.
 * It controls the mixing process of the bioreactor.
 */

#include "MixProgram.h"

MixProgram::MixProgram() : stirringMotor(nullptr), speed(0), running(false), paused(false) {}

void MixProgram::configure(StirringMotor& motor, int speed) {
    this->stirringMotor = &motor;
    this->speed = speed;
}

void MixProgram::begin() {
    this->running = true;
    this->paused = false;
    
    if (stirringMotor) {
        stirringMotor->control(true, speed);
        Serial.println("Mixing started with " + String(stirringMotor->getName()) + " at speed: " + String(speed));
    } else {
        Serial.println("Error: Stirring motor not configured");
    }
}

void MixProgram::update() {
    // Le processus de mélange se poursuit continuellement jusqu'à ce qu'il soit arrêté
    // Aucune logique supplémentaire n'est nécessaire ici
}

void MixProgram::pause() {
    if (running && !paused) {
        stirringMotor->control(false, 0);
        paused = true;
        Serial.println("Mixing paused");
    }
}

void MixProgram::resume() {
    if (running && paused) {
        stirringMotor->control(true, speed);
        paused = false;
        Serial.println("Mixing resumed");
    }
}

void MixProgram::stop() {
    if (running) {
        stirringMotor->control(false, 0);
        running = false;
        paused = false;
        Serial.println("Mixing stopped");
    }
}

bool MixProgram::isRunning() const {
    return running;
}