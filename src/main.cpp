#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include "config.h"

uint8_t *imageBits;

void setup() {
  imageBits = (uint8_t*)malloc(imageSize);
}

void loop() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
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
  
  delay(5*60e3);
}