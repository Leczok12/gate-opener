#include <Arduino.h>
#include "WebSocketClient.h"
#include <WiFi.h>

WebSocketClient ws(true);

void setup()
{
  Serial.begin(115200);
  WiFi.begin("KAMIL_PC", "12345678");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
}

void loop()
{
  if (!ws.isConnected())
  {
    Serial.println("not");
    ws.connect("gate-opener-api.onrender.com", "/ws/XfwgEAtMZDx1VpfxxITwLT9JHptdmnStDNiOdxgdF26Q3IQvOKOcXrBtpboHuKQI", 443);
  }
  else
  {
    // ws.send("hello");

    String msg;
    if (ws.getMessage(msg))
    {
      Serial.println(msg);
    }
    Serial.println("tot");
  }
  delay(500);
}

// #include <Arduino.h>
// #include "config.hpp"
// #include "wifi-manager/wifi-manager.hpp"
// #include "terminal-manager/termina-manager.hpp"
// #include "data-manager/data-manager.hpp"

// void setup()
// {
//   Serial.begin(115200);
//   delay(1000);
//   Serial.print(std::string("\n" + std::string(NAME) + " [" + std::string(VERSION) + "]\n").c_str());
// }

// void loop()
// {
//   DataManager dataManager;
//   WifiManager wifiManager(std::string(NAME) + " " + std::string(VERSION),
//                           dataManager.data["api"].as<std::string>(),
//                           dataManager.data["ssl"].as<std::string>(),
//                           dataManager.data["ssid"].as<std::string>(),
//                           dataManager.data["pass"].as<std::string>());
//   TerminalManager terminalManager(wifiManager, dataManager);
//   wifiManager.connect();
//   unsigned long oldTime = 0;
//   delay(2000);
//   do
//   {
//     terminalManager.tick();
//     if (!terminalManager.isActive() && oldTime + 1000 < millis())
//     {
//       oldTime = millis();
//       wifiManager.tick();
//     }
//   } while (true);
// }
