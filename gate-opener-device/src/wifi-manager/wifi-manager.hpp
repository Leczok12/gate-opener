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

    const std::string api;
    const std::string sslCertificate;
    const std::string ssid;
    const std::string pass;

private:
    void _connect();
    std::string _status(int status);

    int connectionCount;

    std::string macAddres;
    std::string ipV4Addres;
    std::string ipV6Addres;
    std::string gatewayAdress;
    std::string subnetMask;
};
