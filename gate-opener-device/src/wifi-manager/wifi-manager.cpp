#include "wifi-manager.hpp"

WifiManager::WifiManager(std::string name, std::string api, std::string sslCertificate, std::string ssid, std::string pass)
    : api(api),
      sslCertificate(sslCertificate),
      ssid(ssid),
      pass(pass),
      var1(false),
      var2(false),
      connectionCount(0),
      lastFetch(0),
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
    if (connectionCount >= 9999)
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
        if (connectionCount >= 9999)
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

    Serial.print("Timeout 5s\n");
    delay(5000);
};

void WifiManager::tick()
{
    if ((WiFi.status() == WL_CONNECTED) && lastFetch + requestInterval < millis())
    {
        lastFetch = millis();

        HTTPClient http;

        http.begin(String(api.c_str()).c_str());

        int httpCode = http.GET();

        if (httpCode > 0)
        {
            deserializeJson(_lastData, http.getString().c_str());
            if (!_lastData["var1"].isNull())
                var1 = _lastData["var1"].as<bool>();
            if (!_lastData["var2"].isNull())
                var2 = _lastData["var2"].as<bool>();
        }

        else
        {
            Serial.print("Error on HTTP request code [");
            Serial.print(httpCode);
            Serial.print("]\n");
        }
        http.end();
    }
    else if (WiFi.status() != WL_CONNECTED)
    {
        _connect();
    }
};

void WifiManager::sendVar(int n, bool v, bool p)
{
    while (true)
    {
        if (Serial.available())
            if (Serial.read() == 24) // ctr + x
                break;

        if ((WiFi.status() == WL_CONNECTED) && lastFetch + requestInterval < millis())
        {
            lastFetch = millis();

            std::string msg;
            if (n == 1)
                var1 = false;
            else if (n == 2)
                var2 = false;
            else
                return;
            msg = "{\"var" + std::to_string(n) + "\": " + (v ? "true" : "false") + "}";

            uint8_t *payload = (uint8_t *)msg.c_str();
            HTTPClient http;
            http.begin(String(api.c_str()), sslCertificate.c_str());
            http.addHeader("Content-Type", "application/json");

            // THERE ERROR
            int httpCode = http.POST(payload, msg.length());
            http.end();
            if (p)
            {
                if (httpCode == 200)
                    Serial.print("Request was sended\n");
                else
                {
                    Serial.print("Error on HTTP request code [");
                    Serial.print(httpCode);
                    Serial.print("]\n");
                }
            }
            break;
        }
    }
}

void WifiManager::connectionInformation()
{
    Serial.println("[= CONNECTION INFORMATION =]");
    Serial.println(std::string("MAC ADDRES         : " + macAddres).c_str());
    Serial.println(std::string("DEFAULT GAWTEWAY   : " + gatewayAdress).c_str());
    Serial.println(std::string("LOCAL IPv4 ADDRESS : " + ipV4Addres).c_str());
    Serial.println(std::string("LOCAL IPv6 ADDRESS : " + ipV6Addres).c_str());
    Serial.println(std::string("SUBNET MASK        : " + subnetMask).c_str());
}

void WifiManager::lastData()
{
    Serial.print("[= LAST DATA =]\n");
    serializeJsonPretty(_lastData, Serial);
    Serial.print("\n");
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
    return "UNKNOW";
}