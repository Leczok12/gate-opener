#include "wifi-manager.hpp"

WifiManager::WifiManager(std::string name, std::string api, std::string sslCertificate, std::string ssid, std::string pass)
    : api(api),
      sslCertificate(sslCertificate),
      ssid(ssid),
      pass(pass),
      connectionCount(0),
      macAddres(""),
      ipV4Addres(""),
      ipV6Addres(""),
      gatewayAdress(""),
      subnetMask("")
{
    WiFi.setHostname(name.c_str());
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());
    WiFi.setAutoConnect(false);
};

WifiManager::~WifiManager(){};

void WifiManager::connect()
{
    connectionCount = 0;
    _connect();
}
void WifiManager::_connect()
{
    if (connectionCount >= 3)
        return;
    //--[CONNECT TO NETWORK]--
    WiFi.disconnect();
    delay(200);
    macAddres = "";
    ipV4Addres = "";
    ipV6Addres = "";
    gatewayAdress = "";
    subnetMask = "";

    while (WiFi.status() != WL_CONNECTED)
    {
        if (connectionCount >= 3)
            break;

        Serial.print(std::string("\nConnecting [" + ssid + "] ").c_str());
        unsigned long time = millis();
        WiFi.reconnect();
        while (WiFi.status() != WL_CONNECTED)
        {
            if (Serial.available())
            {
                if (Serial.read() == 24) // ctr + x
                {
                    connectionCount = 9999;
                    break;
                }
            }

            if (time + 5000 < millis())
                break;

            Serial.print(".");
            delay(100);
        }
        connectionCount++;
    }
    Serial.print("\n");
    if (WiFi.status() == WL_CONNECTED)
    {
        connectionCount = 0;
        macAddres = WiFi.macAddress().c_str();
        ipV4Addres = WiFi.localIP().toString().c_str();
        ipV6Addres = WiFi.localIPv6().toString().c_str();
        gatewayAdress = WiFi.gatewayIP().toString().c_str();
        subnetMask = WiFi.subnetMask().toString().c_str();

        connectionInformation();
    }
    else
    {
        Serial.print(std::string("Connection failed! [" + _status(WiFi.status()) + "]\n").c_str());
    }
};

void WifiManager::tick()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        // HTTPClient http;

        // http.begin(String(api.c_str()), sslCertificate.c_str()); // Specify the URL and certificate
        // int httpCode = http.GET();                      // Make the request

        // if (httpCode > 0)
        // { // Check for the returning code

        //     String payload = http.getString();
        //     Serial.println(httpCode);
        //     Serial.println(payload);
        // }

        // else
        // {
        //     Serial.println("Error on HTTP request");
        // }

        // http.end();
        // delay(500);
    }
    else
    {
        _connect();
    }
};

void WifiManager::connectionInformation()
{
    Serial.println("[= CONNECTION INFORMATION =]");
    Serial.println(std::string("MAC ADDRES         : " + macAddres).c_str());
    Serial.println(std::string("DEFAULT GAWTEWAY   : " + gatewayAdress).c_str());
    Serial.println(std::string("LOCAL IPv4 ADDRESS : " + ipV4Addres).c_str());
    Serial.println(std::string("LOCAL IPv6 ADDRESS : " + ipV6Addres).c_str());
    Serial.println(std::string("SUBNET MASK        : " + subnetMask).c_str());
}

std::string WifiManager::_status(int status)
{
    switch (status)
    {
    case WL_IDLE_STATUS:
        return "WL_IDLE_STATUS";
    case WL_SCAN_COMPLETED:
        return "WL_SCAN_COMPLETED";
    case WL_NO_SSID_AVAIL:
        return "WL_NO_SSID_AVAIL";
    case WL_CONNECT_FAILED:
        return "WL_CONNECT_FAILED";
    case WL_CONNECTION_LOST:
        return "WL_CONNECTION_LOST";
    case WL_CONNECTED:
        return "WL_CONNECTED";
    case WL_DISCONNECTED:
        return "WL_DISCONNECTED";
    }
}