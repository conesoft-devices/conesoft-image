#include "csft-lib.h"

#include "DEV_Config.h"
#include "EPD.h"

String webRequest;
uint8_t *imageBits;

const int imageSize = (200 * 200) / 8;

unsigned long time_now = 0;
unsigned long period = 60 * 1000;

void setup()
{
  csft_setup("conesoft-web-button", []() -> void
             { webRequest = WiFiSettings.string("webrequest", "", "Conesoft Web Devices Web Request Target"); });

  imageBits = (uint8_t *)malloc(imageSize);
}

void loop()
{

  csft_web_request(webRequest, "Conesoft-Web-Image", [](HTTPClient &http) -> void
  {
    csft_binary_read_response_to(http, imageBits, imageSize);
  });

  DEV_Module_Init();
  EPD_1IN54_V2_Init();
  EPD_1IN54_V2_Display(imageBits);
  EPD_1IN54_V2_Sleep();

  time_now = millis();
  while (millis() < time_now + period)
  {
    delay(10);
    csft_loop();
  }

  ESP.restart();
}