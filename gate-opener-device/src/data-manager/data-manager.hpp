#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

#include "data-manager-config.hpp"

class DataManager
{
public:
    DataManager();
    ~DataManager();

    bool save();
    bool read();

    void reset();

    StaticJsonDocument<4096> data;

private:
};