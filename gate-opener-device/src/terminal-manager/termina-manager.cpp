#include "termina-manager.hpp"

TerminalManager::TerminalManager(WifiManager &wifiManager, DataManager &dataManager)
    : _wifiManager(wifiManager),
      _dataManager(dataManager),
      input(""),
      endInput(false),
      activeInput(false)
{
}

TerminalManager::~TerminalManager()
{
}

bool TerminalManager::isActive()
{
    return activeInput;
}

void TerminalManager::tick()
{
    if (Serial.available())
    {
        activeInput = true;
        std::string s = "";
        while (Serial.available())
        {
            s += Serial.read();
        }

        if (s.length() == 1 && (s[0] >= 32 && s[0] <= 126))
            input += s;
        else if ((s.length() == 1 && (s[0] == 10 || s[0] == 13)) || // "\n" or "\r"
                 (s.length() == 2 && s[0] == 13 && s[1] == 10))     // "\n\r"
            endInput = true;
        else if (s.length() == 1 && s[0] == 8 && input.length() != 0) // backspace
            input.pop_back();

        Serial.print("\33[2K\r");
        Serial.print(std::string("~ : " + input).c_str());
    }

    if (endInput)
    {
        Serial.print('\n');
        endInput = false;
        activeInput = false;
        std::string command = cut(input, 0);
        if (command == "SET")
        {
            command = cut(input, 1);
            if (command == "API")
            {
                _dataManager.data["api"] = cut(input, 2, false);
                Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
            }
            else if (command == "SSL")
            {
                std::string d = largeInput();
                Serial.print("\n");
                if (d != "")
                {
                    _dataManager.data["ssl"] = d;
                    Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
                }
                else
                {
                    Serial.print("- not saved\n");
                }
            }
            else if (command == "SSID")
            {
                _dataManager.data["ssid"] = cut(input, 2, false);
                Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
            }
            else if (command == "PASS")
            {
                _dataManager.data["pass"] = cut(input, 2, false);
                Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
            }
            else
            {
                help("SET");
            }
        }
        else if (command == "GET")
        {
            command = cut(input, 1);
            if (command == "NETWORK")
            {
                _wifiManager.connectionInformation();
            }
            else if (command == "API")
            {
                Serial.print("[= API =]\n");
                Serial.print(_dataManager.data["api"].as<std::string>().c_str());
                Serial.print("\n");
            }
            else if (command == "SSL")
            {
                Serial.print("[= SSL =]\n");
                Serial.print(_dataManager.data["ssl"].as<std::string>().c_str());
                Serial.print("\n");
            }
            else if (command == "SSID")
            {
                Serial.print("[= SSID =]\n");
                Serial.print(_dataManager.data["ssid"].as<std::string>().c_str());
                Serial.print("\n");
            }
            else
            {
                help("GET");
            }
        }
        else if (command == "RESTART")
        {
            command = cut(input, 1);
            if (command == "DEVICE")
                ESP.restart();
            else
                help("RESTART");
        }
        else if (command == "RESET")
        {
            command = cut(input, 1);
            if (command == "DATA")
                _dataManager.reset();
            else
                help("RESET");
        }
        else if (command == "RECONNECT")
            _wifiManager.connect();
        else if (command == "HELP")
            help();
        else
            Serial.print("type help\n");

        input = "";
    }
}

std::string TerminalManager::largeInput()
{
    std::string str = "";
    Serial.print(R"(|-----------------|
| ctrl + x | exit |
| ctrl + s | save |
|-----------------|
)");
    do
    {
        if (Serial.available())
        {
            char c = Serial.read();
            if (c == 24)
            { // ctrl x
                str = "";
                break;
            }
            else if (c == 19) // ctr s
            {
                break;
            }

            if (c >= 32 && c <= 126 || (c == 10 || c == 13))
            {
                str += c;
                Serial.print(c);
            }
        }
    } while (true);
    return str;
}

std::string TerminalManager::cut(std::string data, int n, bool to_upper)
{
    std::string result;
    int wordCount = 0;
    bool inQuotes = false;

    for (char c : data)
    {
        if (c == ' ' && !inQuotes)
        {
            ++wordCount;
            if (wordCount > n)
                break;
            continue;
        }

        if (c == '"')
            inQuotes = !inQuotes;

        if (wordCount == n)
        {
            if (c != '"')
                result += (to_upper) ? std::toupper(c) : c;
        }
    }

    return result;
}

void TerminalManager::help(std::string what)
{
    const std::string SET = R"(
SET
    API  <value> - url to api      
    SSL          - api ssl     
    SSID <value> - network ssid
    PASS <value> - network password      
)";
    const std::string GET = R"(
GET
    NETWORK - network inforamtion
    API     - url to api      
    SSL     - api ssl     
    SSID    - network ssid
)";
    const std::string RESTART = R"(
RESTART
    DEVICE - restart device
)";
    const std::string RESET = R"(
RESET
    DATA - RESET DATA
)";
    const std::string RECONNECT = R"(
RECONNECT - reconnect to network
)";

    Serial.print("[= HELP =]\n\r");
    if (what == "" || what == "SET")
        Serial.print(SET.c_str());
    if (what == "" || what == "GET")
        Serial.print(GET.c_str());
    if (what == "" || what == "RESTART")
        Serial.print(RESTART.c_str());
    if (what == "" || what == "RESET")
        Serial.print(RESET.c_str());
    if (what == "" || what == "RECONNECT")
        Serial.print(RECONNECT.c_str());
}