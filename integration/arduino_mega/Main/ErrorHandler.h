#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <Arduino.h>
#include <logger/Logger.h>

class ErrorHandler {
public:
    static void handleError(const String& errorMessage);
    static void checkSensorResponses();
    static void checkActuatorResponses();
};

#endif