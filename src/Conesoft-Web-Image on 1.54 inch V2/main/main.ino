#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char *ssid = ""; //"your ssid";
const char *password = "";   //"your password";
const size_t imageSize = (200 * 200) / 8;
const char *webAddress = "http:///device/image";

void setup() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  uint8_t *imageBits = (uint8_t*)malloc(imageSize);
  WiFiClient wificlient;
  HTTPClient http;
  if(http.begin(wificlient, webAddress) == false) {
    return; // error
  }
  http.setUserAgent("Conesoft-Web-Image");
  http.addHeader("Conesoft-Web-Image-Id", WiFi.macAddress());
  http.GET();
  wificlient = http.getStream();
  size_t length = http.getSize();
  for(size_t index = 0; index < length; index) {
    index += wificlient.readBytes(imageBits + index, imageSize - index);    
  }
  wificlient.stop();
  http.end();
  WiFi.disconnect();
  
  DEV_Module_Init();
  EPD_1IN54_V2_Init();
  EPD_1IN54_V2_Clear();
  //DEV_Delay_ms(500);
  EPD_1IN54_V2_Display(imageBits);
  //DEV_Delay_ms(1000);
  EPD_1IN54_V2_Sleep();
  free(imageBits);
  imageBits = NULL;
}

void loop() {
}