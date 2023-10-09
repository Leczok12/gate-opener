#pragma once
#include <Arduino.h>
#include "../wifi-manager/wifi-manager.hpp"
#include "../data-manager/data-manager.hpp"

class TerminalManager
{
public:
    TerminalManager(
        WifiManager &wifiManager,
        DataManager &dataManager);
    ~TerminalManager();

    bool isActive();
    void tick();

private:
    WifiManager &_wifiManager;
    DataManager &_dataManager;
    std::string input;
    bool endInput;
    bool activeInput;

    std::string largeInput();
    std::string cut(std::string data, int n, bool to_upper = true);
    void help(std::string what = "");
};
