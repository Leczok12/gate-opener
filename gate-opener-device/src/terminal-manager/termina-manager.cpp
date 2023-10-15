#include "termina-manager.hpp"

TerminalManager::TerminalManager(DataManager &dataManager)
    : _dataManager(dataManager),
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

TERMINAL_ACTION TerminalManager::loop()
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
        std::string c = input;
        input = "";
        return command(c);
    }
    return NONE;
}

TERMINAL_ACTION TerminalManager::command(std::string _command)
{

    std::string command = cut(_command, 0);
    if (command == "SET")
    {
        command = cut(_command, 1);
        if (command == "DOMAIN")
        {
            _dataManager.data["domain"] = cut(_command, 2, false);
            Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
        }
        else if (command == "PORT")
        {
            _dataManager.data["port"] = cut(_command, 2, false);
            Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
        }
        else if (command == "PATH")
        {
            std::string d = largeInput();
            Serial.print("\n");
            if (d != "")
            {
                _dataManager.data["path"] = d;
                Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
            }
            else
            {
                Serial.print("- not saved\n");
            }
        }
        else if (command == "SSID")
        {
            _dataManager.data["ssid"] = cut(_command, 2, false);
            Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
        }
        else if (command == "PASS")
        {
            _dataManager.data["pass"] = cut(_command, 2, false);
            Serial.print(_dataManager.save() ? "- saved\n" : "- not saved\n");
        }
        else
        {
            help("SET");
        }
    }
    else if (command == "GET")
    {
        command = cut(_command, 1);
        if (command == "NETWORK")
        {
            Serial.print("[= NETWORK =]\n");
            Serial.print(std::string("MAC ADDRES         : " + std::string(WiFi.macAddress().c_str()) + "\n").c_str());
            Serial.print(std::string("DEFAULT GAWTEWAY   : " + std::string(WiFi.gatewayIP().toString().c_str()) + "\n").c_str());
            Serial.print(std::string("LOCAL IPv4 ADDRESS : " + std::string(WiFi.localIP().toString().c_str()) + "\n").c_str());
            Serial.print(std::string("LOCAL IPv6 ADDRESS : " + std::string(WiFi.localIPv6().toString().c_str()) + "\n").c_str());
            Serial.print(std::string("SUBNET MASK        : " + std::string(WiFi.subnetMask().toString().c_str()) + "\n").c_str());
        }
        else if (command == "LASTDATA")
        {
            return GET_LAST_DATA;
        }
        else if (command == "DOMAIN")
        {
            Serial.print("[= DOMAIN =]\n");
            Serial.print(_dataManager.data["domain"].as<std::string>().c_str());
            Serial.print("\n");
        }
        else if (command == "PORT")
        {
            Serial.print("[= PORT =]\n");
            Serial.print(_dataManager.data["port"].as<std::string>().c_str());
            Serial.print("\n");
        }
        else if (command == "PATH")
        {
            Serial.print("[= PATH =]\n");
            Serial.print(_dataManager.data["path"].as<std::string>().c_str());
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
        command = cut(_command, 1);
        if (command == "DEVICE")
            ESP.restart();
        else
            help("RESTART");
    }
    else if (command == "RESET")
    {
        command = cut(_command, 1);
        if (command == "DATA")
            _dataManager.reset();
        else
            help("RESET");
    }
    else if (command == "TEST")
    {
        command = cut(_command, 1);
        if (command == "1")
        {
            command = cut(_command, 2);
            if (command == "TRUE")
                return TEST_1_TRUE;
            else if (command == "FALSE")
                return TEST_1_FALSE;
            else
                help("TEST");
        }
        else if (command == "2")
        {
            command = cut(_command, 2);
            if (command == "TRUE")
                return TEST_2_TRUE;
            else if (command == "FALSE")
                return TEST_2_FALSE;
            else
                help("TEST");
        }
        else
        {
            help("TEST");
        }
    }
    else if (command == "RECONNECT")
        return RECONNECT;
    else if (command == "HELP")
        help();
    else
        Serial.print("type help\n");

    return NONE;
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
    DOMAIN <value> - domain      
    PORT   <value> - domain port     
    PATH           - path with key    
    SSID   <value> - network ssid
    PASS   <value> - network password      
)";
    const std::string GET = R"(
GET
    NETWORK  - network inforamtion
    DOMAIN   - domain      
    PORT     - domain port     
    KEY      - domain key     
    SSID     - network ssid
    LASTDATA - last fetched data
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
    const std::string TEST = R"(
TEST <1 , 2> <false , true> - test api
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
    if (what == "" || what == "TEST")
        Serial.print(TEST.c_str());
}