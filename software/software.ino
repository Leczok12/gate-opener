#include <RMaker.h>
#include <WiFi.h>
#include <WiFiProv.h>
#include <AppInsights.h>
#include <Adafruit_NeoPixel.h>

#include "config.hpp"
#include "QuickButton.h"

QuickButton button(GPIO_PIN_BUTTON);
Adafruit_NeoPixel pixels(3, GPIO_PIN_LED, NEO_GRB + NEO_KHZ800);
bool lock = false; // by button
bool remoteConnected = false; // by unplug remote conntroller

struct _Switch{
  Switch * instance;
  bool active;
  unsigned long activeTime;
  int pin;
};

_Switch sw1 = {nullptr, false, 0, GPIO_PIN_SWITCH_1};
_Switch sw2 = {nullptr, false, 0, GPIO_PIN_SWITCH_2};
_Switch sw3 = {nullptr, false, 0, GPIO_PIN_SWITCH_3};
_Switch sw4 = {nullptr, false, 0, GPIO_PIN_SWITCH_4};

// static Switch *sw1 = nullptr; 
// static Switch *sw2 = nullptr; 
// static Switch *sw3 = nullptr; 
// static Switch *sw4 = nullptr; 

// bool sw1_start = false;
// bool sw2_start = false;
// bool sw3_start = false;
// bool sw4_start = false;

// unsigned long sw1_time = 0;
// unsigned long sw2_time = 0;
// unsigned long sw3_time = 0;
// unsigned long sw4_time = 0;

void sysProvEvent(arduino_event_t *sys_event){
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n",SERVICE_NAME, POP);
        printQR(SERVICE_NAME, POP, "ble");
        break;
    case ARDUINO_EVENT_PROV_INIT:
        wifi_prov_mgr_disable_auto_stop(10000);
        break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
        wifi_prov_mgr_stop_provisioning();
        break;
    default:;
  }
};

void switchCallback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx){
    const char *device_name = device->getDeviceName();
    const char *param_name = param->getParamName();

    if(strcmp(device_name, "SW_1") == 0){
      sw1.active = (val.val.b ? true : false);
      param->updateAndReport(val);
    }
    else if(strcmp(device_name, "SW_2") == 0){
      sw2.active = (val.val.b ? true : false);
      param->updateAndReport(val);
    }
    else if(strcmp(device_name, "SW_3") == 0){
      sw3.active = (val.val.b ? true : false);
      param->updateAndReport(val);
    }
    else if(strcmp(device_name, "SW_4") == 0){
      sw4.active = (val.val.b ? true : false);
      param->updateAndReport(val);
    };
};

void buttonCallback(unsigned long duration, int count){
  if(duration > 15000){
    Serial.printf("Reset to factory.\n");
    blink(INDEX_LED_POWER);
    RMakerFactoryReset(0);
  }
  else if(duration > 5000){
    Serial.printf("Reset Wi-Fi.\n");
    blink(INDEX_LED_WIFI);
    RMakerWiFiReset(0);
  }
  else if(count == 2){
    lock = !lock;
  }
};

void blink(int pixel){
  auto oldPixel = pixels.getPixelColor(pixel);
  for (int i = 0; i < 5; i++) {
    pixels.setPixelColor(pixel, i%2 == 0 ? 0x000000 : 0xFFFFFF);
    pixels.show();
    delay(300);
  }
  pixels.setPixelColor(pixel,oldPixel);
  pixels.show();
};

bool areSwitchesLow(){
  if(digitalRead(sw1.pin) == LOW && digitalRead(sw2.pin) == LOW && digitalRead(sw3.pin) == LOW && digitalRead(sw4.pin) == LOW)
    return true;
  return false;
};

void switchLoop(_Switch &sw){
  if (areSwitchesLow() && sw.active) {
    sw.active = false;
    if(remoteConnected && !lock){
      sw.activeTime = millis();
      digitalWrite(sw.pin, HIGH);
    }
    else{
      sw.activeTime = 0;
      digitalWrite(sw.pin, LOW);
      sw.instance->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    }
  };

  if(sw.activeTime != 0 && sw.activeTime + VIRTUAL_PRESS_TIME < millis()){
    sw.activeTime = 0;
    digitalWrite(sw.pin, LOW);
    sw.instance->updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
  };
};

void setup() {
  Serial.begin(115200);

  button.onClick(buttonCallback);

  pinMode(sw1.pin, OUTPUT);
  pinMode(sw2.pin, OUTPUT);
  pinMode(sw3.pin, OUTPUT);
  pinMode(sw4.pin, OUTPUT);
  pinMode(GPIO_PIN_REMOTE_CONNECTED, INPUT_PULLUP);

  Node node;
  node = RMaker.initNode(NODE_NAME);

  sw1.instance = new Switch("SW_1", &GPIO_PIN_SWITCH_1, false);
  sw1.instance->addCb(switchCallback);
  node.addDevice(*sw1.instance);

  sw2.instance = new Switch("SW_2", &GPIO_PIN_SWITCH_2, false);
  sw2.instance->addCb(switchCallback);
  node.addDevice(*sw2.instance);

  sw3.instance = new Switch("SW_3", &GPIO_PIN_SWITCH_3, false);
  sw3.instance->addCb(switchCallback);
  node.addDevice(*sw3.instance);

  sw4.instance = new Switch("SW_4", &GPIO_PIN_SWITCH_4, false);
  sw4.instance->addCb(switchCallback);
  node.addDevice(*sw4.instance);

  RMaker.enableOTA(OTA_USING_TOPICS);
  RMaker.enableTZService();
  RMaker.enableSchedule();
  RMaker.enableScenes();
  initAppInsights();
  RMaker.enableSystemService(SYSTEM_SERV_FLAGS_ALL, 2, 2, 2);
  RMaker.start();
  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, POP, SERVICE_NAME);

  pixels.begin();
  pixels.clear();
  pixels.show();
};

unsigned long lastBlinkTime = 0;
bool blinkOn = false;

void loop() {
  button.loop();
  digitalRead(GPIO_PIN_REMOTE_CONNECTED) == HIGH ? remoteConnected = false : remoteConnected = true;

  if(lastBlinkTime + BLINK_INTERVAL < millis()){
    //POWER LED
    pixels.setPixelColor(INDEX_LED_POWER,pixels.Color(0,255,0));
    
    //WIFI LED
    pixels.setPixelColor(INDEX_LED_WIFI, (blinkOn && WiFi.isConnected() ? 0xFFAA00 : 0x0));
    
    //WORKING LED
    if(!areSwitchesLow())
      pixels.setPixelColor(INDEX_LED_WORKING, (blinkOn ? 0x0000FF : 0x0));
    else if(lock)
      pixels.setPixelColor(INDEX_LED_WORKING, 0xFF0000);
    else if(!remoteConnected)
      pixels.setPixelColor(INDEX_LED_WORKING, (blinkOn ? 0xFF0000 : 0x0));
    else
      pixels.setPixelColor(INDEX_LED_WORKING, 0x0);

    pixels.setBrightness(60);
    pixels.show();
    lastBlinkTime = millis();
    blinkOn = !blinkOn;
  }

  switchLoop(sw1);
  switchLoop(sw2);
  switchLoop(sw3);
  switchLoop(sw4);
};
