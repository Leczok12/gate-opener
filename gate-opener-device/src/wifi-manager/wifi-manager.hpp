#pragma once
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

class WifiManager
{
public:
    WifiManager(
        std::string name,
        std::string api,
        std::string sslCertificate,
        std::string ssid,
        std::string pass);
    ~WifiManager();

    void connect();
    void tick();

    void connectionInformation();
    void lastData();

    const std::string api;
    const std::string sslCertificate;
    const std::string ssid;
    const std::string pass;

    bool var1;
    bool var2;

    void sendVar(int n, bool v = false, bool p = false);

private:
    StaticJsonDocument<128> _lastData;
    void _connect();
    std::string _status(int status);
    HTTPClient http;

    int connectionCount;
    unsigned long lastFetch;

    std::string macAddres;
    std::string ipV4Addres;
    std::string ipV6Addres;
    std::string gatewayAdress;
    std::string subnetMask;
};
