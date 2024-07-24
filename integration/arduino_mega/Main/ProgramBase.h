/*
 * ProgramBase.h
 * This file defines the base class for all programs in the bioreactor system.
 * It provides a common interface for all programs to implement.
 */

#ifndef PROGRAM_BASE_H
#define PROGRAM_BASE_H

#include <Arduino.h>

class ProgramBase {
public:
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void pause() = 0;
    virtual void resume() = 0;
    virtual void stop() = 0;
    virtual bool isRunning() const = 0;
    virtual String getName() const = 0;
    virtual ~ProgramBase() {}
};

#endif // PROGRAM_BASE_H