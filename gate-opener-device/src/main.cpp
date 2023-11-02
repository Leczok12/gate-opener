#include "config.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ESP32Servo.h>

#include "data-manager/data-manager.hpp"
#include "terminal-manager/termina-manager.hpp"

WebSocketsClient ws;
Servo servo;

unsigned long oldTimeLedConnected = 0;
unsigned long oldTimeLedWorking = 0;
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
    if (connectionCount >= MAX_CONNECTION_COUNT)
        return;

    if (force)
    {
        ws.disconnect();
        WiFi.disconnect();
        delay(200);
    }

    if (WiFi.status() != WL_CONNECTED && connectionCount < MAX_CONNECTION_COUNT)
    {
        while (WiFi.status() != WL_CONNECTED && connectionCount < MAX_CONNECTION_COUNT)
        {
            WiFi.begin(dataManager.data["ssid"].as<std::string>().c_str(),
                       dataManager.data["pass"].as<std::string>().c_str());

            Serial.print(std::string("Connecting [" + dataManager.data["ssid"].as<std::string>() + "] => ").c_str());
            unsigned long oldTime = millis();
            while (WiFi.status() != WL_CONNECTED && connectionCount < MAX_CONNECTION_COUNT)
            {
                if (oldTime + 5000 < millis())
                    break;

                if (Serial.available() && Serial.read() == 24) // ctr + x
                    connectionCount = MAX_CONNECTION_COUNT;

                Serial.print(".");
                delay(200);
            }
            Serial.print("\n");
            connectionCount++;
        }
        if (WiFi.status() == WL_CONNECTED)
            terminalManager.command("GET NETWORK");
    }
    if (!ws.isConnected() && connectionCount < MAX_CONNECTION_COUNT)
    {
        while (!ws.isConnected() && connectionCount < MAX_CONNECTION_COUNT)
        {
            ws.disconnect();
            ws = WebSocketsClient();
            ws.onEvent(_wsEvent);
            ws.beginSSL(dataManager.data["domain"].as<std::string>().c_str(),
                        dataManager.data["port"].as<int>(),
                        dataManager.data["path"].as<std::string>().c_str());
            Serial.print(std::string("Connecting [" + dataManager.data["domain"].as<std::string>() + "] => ").c_str());
            unsigned long oldTime = millis();
            while (!ws.isConnected() && connectionCount < MAX_CONNECTION_COUNT)
            {
                if (oldTime + 5000 < millis())
                    break;

                if (Serial.available() && Serial.read() == 24) // ctr + x
                    connectionCount = MAX_CONNECTION_COUNT;

                Serial.print(".");
                ws.loop();
                delay(200);
            }
            Serial.print("\n");
            connectionCount++;
        }
    }
    connectionCount = 0;
}

void setup()
{
    Serial.begin(115200);

    delay(1000);

    // ESP32PWM::allocateTimer(0);
    // ESP32PWM::allocateTimer(1);
    // ESP32PWM::allocateTimer(2);
    // ESP32PWM::allocateTimer(3);
    servo.setPeriodHertz(50);
    servo.attach(PIN_SERVO_MOTOR, 500, 2400);
    servo.write(90);

    pinMode(PIN_LED_CONNECTED, OUTPUT);
    digitalWrite(PIN_LED_CONNECTED, LOW);
    pinMode(PIN_LED_WORKING, OUTPUT);
    digitalWrite(PIN_LED_WORKING, LOW);

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
                unsigned long startTime = millis();
                digitalWrite(PIN_LED_CONNECTED, LOW);
                servo.write(90 - dataManager.data["servo_tilt"].as<int>());
                while (startTime + 2000 > millis())
                {
                    ws.loop();
                    if (oldTimeLedWorking + BLINK_WORKING_INTERWAL < millis())
                    {
                        oldTimeLedWorking = millis();
                        digitalWrite(PIN_LED_WORKING, !digitalRead(PIN_LED_WORKING));
                    }
                }
                servo.write(90);
                var1 = false;
                ws.sendTXT(R"({"var1":false})");
            }
            if (var2)
            {
                unsigned long startTime = millis();
                digitalWrite(PIN_LED_CONNECTED, LOW);
                servo.write(90 + dataManager.data["servo_tilt"].as<int>());
                while (startTime + 2000 > millis())
                {
                    ws.loop();
                    if (oldTimeLedWorking + BLINK_WORKING_INTERWAL < millis())
                    {
                        oldTimeLedWorking = millis();
                        digitalWrite(PIN_LED_WORKING, !digitalRead(PIN_LED_WORKING));
                    }
                }
                servo.write(90);
                var2 = false;
                ws.sendTXT(R"({"var2":false})");
            }
            digitalWrite(PIN_LED_WORKING, LOW);

            if (oldTimeLedConnected + BLINK_CONNECTED_INTERWAL < millis())
            {
                oldTimeLedConnected = millis();
                digitalWrite(PIN_LED_CONNECTED, !digitalRead(PIN_LED_CONNECTED));
            }

            if (WiFi.status() != WL_CONNECTED || !ws.isConnected())
            {
                digitalWrite(PIN_LED_CONNECTED, LOW);
                _connect(dataManager, terminalManager);
            }
        }
    }
}