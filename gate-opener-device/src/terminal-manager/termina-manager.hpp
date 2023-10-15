#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include "../data-manager/data-manager.hpp"

enum TERMINAL_ACTION
{
    NONE,
    RECONNECT,
    GET_LAST_DATA,
    TEST_1_TRUE,
    TEST_1_FALSE,
    TEST_2_TRUE,
    TEST_2_FALSE,
};

class TerminalManager
{
public:
    TerminalManager(DataManager &dataManager);
    ~TerminalManager();

    bool isActive();
    TERMINAL_ACTION loop();
    TERMINAL_ACTION command(std::string);

private:
    DataManager &_dataManager;
    std::string input;
    bool endInput;
    bool activeInput;

    std::string largeInput();
    std::string cut(std::string data, int n, bool to_upper = true);
    void help(std::string what = "");
};
