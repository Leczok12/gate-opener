#pragma once

// DATA
const char *NAME = "GATE OPENER";
const char *VERSION = "1.0.0";

constexpr int MAX_CONNECTION_COUNT = 999;
constexpr int BLINK_CONNECTED_INTERWAL = 300;
constexpr int BLINK_WORKING_INTERWAL = 100;
constexpr int MAX_TILT = 15;
// PINS
constexpr int PIN_SERVO_MOTOR = 16;
constexpr int PIN_LED_WORKING = 17;
constexpr int PIN_LED_CONNECTED = 18;