#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DoubleResetDetector.h>
#include <WiFiSettings.h>
#include <LittleFS.h>
#include "config.h"

DoubleResetDetector drd(10, 0);

uint8_t *imageBits;

void setup() {
  system_update_cpu_freq(160);
  Serial.begin(115200);
  Serial.println("booting up conesoft-web-image");
  pinMode(LED_BUILTIN, OUTPUT);
  LittleFS.begin();

  WiFiSettings.hostname = "csft-img-";
  String host = WiFiSettings.string("host", "", "Conesoft Web Devices Server");
  int port = WiFiSettings.integer("port", 0, 65535, 0, "Conesoft Web Devices Server Port");

  if (drd.detectDoubleReset()) {
    WiFiSettings.portal();
  }

  WiFiSettings.connect();

  imageBits = (uint8_t*)malloc(imageSize);
}

unsigned long time_now = 0;
unsigned long period = 60 * 1000;

void loop() {
  WiFiClient wificlient;
  HTTPClient http;
  if(http.begin(wificlient, webrequest) == false) {
    return; // error
  }
  http.setUserAgent("Conesoft-Web-Image");
  http.addHeader("Conesoft-Web-Image-Id", WiFi.macAddress());
  http.GET();
  wificlient = http.getStream();
  size_t length = http.getSize();
  for(size_t index = 0; index < length; index += 0) {
    index += wificlient.readBytes(imageBits + index, imageSize - index);    
  }
  wificlient.stop();
  http.end();
  WiFi.disconnect();

  DEV_Module_Init();
  EPD_1IN54_V2_Init();
  EPD_1IN54_V2_Display(imageBits);
  EPD_1IN54_V2_Sleep();

  time_now = millis();

  while(millis() < time_now + period) {
    delay(10);
    drd.loop();
  }

  ESP.restart();
}