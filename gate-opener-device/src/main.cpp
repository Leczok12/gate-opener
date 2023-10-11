#include <Arduino.h>
#include "config.hpp"
#include "wifi-manager/wifi-manager.hpp"
#include "terminal-manager/termina-manager.hpp"
#include "data-manager/data-manager.hpp"

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.print(std::string("\n" + std::string(NAME) + " [" + std::string(VERSION) + "]\n").c_str());
}

void loop()
{
  // WifiManager wifiManager("ABAKS_INTERNET_2.4G", "!internet!");
  DataManager dataManager;
  WifiManager wifiManager(std::string(NAME) + " " + std::string(VERSION),
                          dataManager.data["api"].as<std::string>(),
                          dataManager.data["ssl"].as<std::string>(),
                          dataManager.data["ssid"].as<std::string>(),
                          dataManager.data["pass"].as<std::string>());
  TerminalManager terminalManager(wifiManager, dataManager);
  wifiManager.connect();
  unsigned long oldTime = 0;
  delay(2000);
  do
  {
    terminalManager.tick();
    if (!terminalManager.isActive() && oldTime + 1000 < millis())
    {
      oldTime = millis();
      wifiManager.tick();
    }
  } while (true);
}