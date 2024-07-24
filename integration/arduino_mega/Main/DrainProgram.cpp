/*
 * DrainProgram.cpp
 * This file implements the DrainProgram class defined in DrainProgram.h.
 * It controls the draining process of the bioreactor.
 */

#include "DrainProgram.h"

DrainProgram::DrainProgram() : drainPump(nullptr), rate(0), duration(0), startTime(0), running(false), paused(false) {}

void DrainProgram::configure(DCPump& pump, int rate, int duration) {
    this->drainPump = &pump;
    this->rate = rate;
    this->duration = duration;
}

void DrainProgram::begin() {
    this->running = true;
    this->paused = false;
    this->startTime = millis();
    
    if (drainPump) {
        drainPump->control(true, rate);
        Serial.println("Drain started at rate: " + String(rate));
    } else {
        Serial.println("Error: Drain pump not configured");
    }
}

void DrainProgram::update() {
    if (!running || paused) return;

    if (millis() - startTime >= duration * 1000UL) {
        stop();
    }
}

void DrainProgram::pause() {
    if (running && !paused) {
        drainPump->control(false, 0);
        paused = true;
        Serial.println("Drain paused");
    }
}

void DrainProgram::resume() {
    if (running && paused) {
        drainPump->control(true, rate);
        paused = false;
        Serial.println("Drain resumed");
    }
}

void DrainProgram::stop() {
    if (running) {
        drainPump->control(false, 0);
        running = false;
        paused = false;
        Serial.println("Drain finished");
    }
}

bool DrainProgram::isRunning() const {
    return running;
}