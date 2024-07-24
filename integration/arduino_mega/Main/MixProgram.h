/*
 * MixProgram.h
 * This file defines the MixProgram class which controls the mixing process of the bioreactor.
 */

#ifndef MIX_PROGRAM_H
#define MIX_PROGRAM_H

#include "ProgramBase.h"
#include "StirringMotor.h"

class MixProgram : public ProgramBase {
public:
    MixProgram();
    void begin() override;
    void configure(StirringMotor& motor, int speed);
    void update() override;
    void pause() override;
    void resume() override;
    void stop() override;
    bool isRunning() const override;
    String getName() const override { return "Mix"; }

private:
    StirringMotor* stirringMotor;
    int speed;
    bool running;
    bool paused;
};

#endif // MIX_PROGRAM_H