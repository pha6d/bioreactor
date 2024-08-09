/*
 * TurbiditySensorSEN0554.cpp
 * This file provides the implementation of the TurbiditySensorSEN0554 class defined in TurbiditySensorSEN0554.h.
 * The class reads turbidity values from a UART non-contact optical turbidity sensor (SEN0554).
 *
 * Reference: https://wiki.dfrobot.com/SKU_SEN0554_Turbidity_Sensor
 */

#include "TurbiditySensorSEN0554.h"
#include "Logger.h"

TurbiditySensorSEN0554::TurbiditySensorSEN0554(int rxPin, int txPin, const char* name) 
    : _serial(rxPin, txPin), _name(name) {}

void TurbiditySensorSEN0554::begin() {
    _serial.begin(9600);  // Baud rate is 9600 as per sensor specifications
    Logger::log(LogLevel::INFO, String(_name) + " initialized");
}

float TurbiditySensorSEN0554::readValue() {
    if (communicate()) {
        return static_cast<float>(_response[3]);
    }
    return -1.0f; // Return an error value if communication fails
}

bool TurbiditySensorSEN0554::communicate() {
    _serial.write(_command, 5);
    
    unsigned long startTime = millis();
    while (!_serial.available()) {
        if (millis() - startTime > 1000) {
            Logger::log(LogLevel::ERROR, String(_name) + " communication timeout");
            return false;
        }
    }

    if (_serial.available() >= 5) {
        for (int i = 0; i < 5; i++) {
            _response[i] = _serial.read();
        }
        _serial.flush();
        return true;
    }

    Logger::log(LogLevel::ERROR, String(_name) + " incomplete response");
    return false;
}