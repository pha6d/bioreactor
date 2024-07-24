#ifndef DRAIN_PROGRAM_H
#define DRAIN_PROGRAM_H

#include "ProgramBase.h"
#include "DCPump.h"

class DrainProgram : public ProgramBase {
public:
    DrainProgram();
    void configure(DCPump& pump, int rate, int duration);
    void begin() override;
    void update() override;
    void pause() override;
    void resume() override;
    void stop() override;
    bool isRunning() const override;
    String getName() const override { return "Drain"; }

private:
    DCPump* drainPump;
    int rate;
    int duration;
    unsigned long startTime;
    bool running;
    bool paused;
};

#endif // DRAIN_PROGRAM_H