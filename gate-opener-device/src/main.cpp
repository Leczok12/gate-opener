#include "config.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>

#include "data-manager/data-manager.hpp"
#include "terminal-manager/termina-manager.hpp"

WebSocketsClient ws;

int connectionCount = 0;
StaticJsonDocument<128> lastData;

bool var1 = false;
bool var2 = false;

void _wsEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_DISCONNECTED:
        Serial.print("ws disconnected\n");
        break;
    case WStype_CONNECTED:
        Serial.print("\nws connected\n");
        break;
    case WStype_TEXT:
    {
        auto e = deserializeJson(lastData, std::string((char *)payload));
        if (e == DeserializationError::Ok)
        {
            if (!lastData["var1"].isNull())
                var1 = lastData["var1"].as<bool>();
            if (!lastData["var2"].isNull())
                var2 = lastData["var2"].as<bool>();
        }
        else
            Serial.print(std::string("ws invalid data [" + std::string(e.c_str()) + "]\n").c_str());
    }
    break;
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
        break;
    }
}

void _connect(DataManager &dataManager, TerminalManager &terminalManager, bool force = false)
{
    if (connectionCount >= 9999)
        return;

    if (force)
    {
        ws.disconnect();
        WiFi.disconnect();
        delay(200);
    }

    if (WiFi.status() != WL_CONNECTED && connectionCount < 9999)
    {
        while (WiFi.status() != WL_CONNECTED && connectionCount < 9999)
        {
            WiFi.begin(dataManager.data["ssid"].as<std::string>().c_str(),
                       dataManager.data["pass"].as<std::string>().c_str());

            Serial.print(std::string("Connecting [" + dataManager.data["ssid"].as<std::string>() + "] => ").c_str());
            unsigned long oldTime = millis();
            while (WiFi.status() != WL_CONNECTED && connectionCount < 9999)
            {
                if (oldTime + 5000 < millis())
                    break;

                if (Serial.available() && Serial.read() == 24) // ctr + x
                    connectionCount = 9999;

                Serial.print(".");
                delay(200);
            }
            Serial.print("\n");
            connectionCount++;
        }
        if (WiFi.status() == WL_CONNECTED)
            terminalManager.command("GET NETWORK");
    }
    if (!ws.isConnected() && connectionCount < 9999)
    {
        while (!ws.isConnected() && connectionCount < 9999)
        {
            ws.disconnect();
            ws = WebSocketsClient();
            ws.onEvent(_wsEvent);
            ws.beginSSL(dataManager.data["domain"].as<std::string>().c_str(),
                        dataManager.data["port"].as<int>(),
                        dataManager.data["path"].as<std::string>().c_str());
            Serial.print(std::string("Connecting [" + dataManager.data["domain"].as<std::string>() + "] => ").c_str());
            unsigned long oldTime = millis();
            while (!ws.isConnected() && connectionCount < 9999)
            {
                if (oldTime + 5000 < millis())
                    break;

                if (Serial.available() && Serial.read() == 24) // ctr + x
                    connectionCount = 9999;

                Serial.print(".");
                ws.loop();
                delay(200);
            }
            Serial.print("\n");
            connectionCount++;
        }
    }
}

void setup()
{
    Serial.begin(115200);

    delay(1000);
    Serial.print(std::string("\n" + std::string(NAME) + " [" + std::string(VERSION) + "]\n").c_str());
    WiFi.setHostname(std::string(std::string(NAME) + std::string(VERSION)).c_str());
    WiFi.setAutoConnect(false);
    WiFi.mode(WIFI_MODE_STA);
}

void loop()
{
    DataManager dataManager;
    TerminalManager terminalManager(dataManager);

    while (true)
    {

        switch (terminalManager.loop())
        {
        case TERMINAL_ACTION::RECONNECT:
            connectionCount = 0;
            _connect(dataManager, terminalManager, true);
            break;
        case TERMINAL_ACTION::GET_LAST_DATA:
            serializeJsonPretty(lastData, Serial);
            break;
        case TERMINAL_ACTION::TEST_1_FALSE:
            ws.sendTXT(R"({"var1":false})");
            break;
        case TERMINAL_ACTION::TEST_1_TRUE:
            ws.sendTXT(R"({"var1":true})");
            break;
        case TERMINAL_ACTION::TEST_2_FALSE:
            ws.sendTXT(R"({"var2":false})");
            break;
        case TERMINAL_ACTION::TEST_2_TRUE:
            ws.sendTXT(R"({"var2":true})");
            break;
        default:
            break;
        }

        if (!terminalManager.isActive())
        {
            ws.loop();
            if (var1)
            {
                delay(2000);
                ws.sendTXT(R"({"var1":false})");
            }
            if (var2)
            {
                delay(2000);
                ws.sendTXT(R"({"var2":false})");
            }

            if (WiFi.status() != WL_CONNECTED || !ws.isConnected())
                _connect(dataManager, terminalManager);
        }
    }
}