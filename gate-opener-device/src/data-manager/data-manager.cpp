#include "data-manager.hpp"

DataManager::DataManager()
    : data()
{
    if (!SPIFFS.begin(true))
    {
        return;
    }
    if (!read())
        reset();
}

DataManager::~DataManager() {}

bool DataManager::save()
{
    File file = SPIFFS.open("/config.json", "w");
    if (!file)
    {
        return false;
    }
    serializeJson(data, file);
    file.close();
    return true;
}

bool DataManager::read()
{
    File file = SPIFFS.open("/config.json", "r");
    if (!file)
    {
        return false;
    }
    if (deserializeJson(data, file) != DeserializationError::Ok)
    {
        file.close();
        return false;
    }
    file.close();

    if (
        data["domain"].isNull() ||
        data["port"].isNull() ||
        data["path"].isNull() ||
        data["ssid"].isNull() ||
        data["pass"].isNull())
    {
        return false;
    }
    return true;
}

void DataManager::reset()
{

    File file = SPIFFS.open("/config.json", "w", true);
    if (deserializeJson(data, PRIMARY_DATA) == DeserializationError::Ok)
    {
        serializeJson(data, file);
    }
    file.close();
}