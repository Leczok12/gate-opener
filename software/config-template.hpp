// Variables
constexpr const char *SERVICE_NAME = ""; // REQUIRED
constexpr const char *POP = "";          // REQUIRED

constexpr unsigned long VIRTUAL_PRESS_TIME = 2000; // in miliseconds
constexpr unsigned long BLINK_INTERVAL = 300;      // in miliseconds

constexpr const char *NODE_NAME = "controller";

constexpr int INDEX_LED_POWER = 0;
constexpr int INDEX_LED_WIFI = 1;
constexpr int INDEX_LED_WORKING = 2;

// Pins
constexpr int GPIO_PIN_BUTTON = 27;
constexpr int GPIO_PIN_LED = 13;
constexpr int GPIO_PIN_REMOTE_CONNECTED = 12;

int GPIO_PIN_SWITCH_1 = 32;
int GPIO_PIN_SWITCH_2 = 33;
int GPIO_PIN_SWITCH_3 = 25;
int GPIO_PIN_SWITCH_4 = 26;