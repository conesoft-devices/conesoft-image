#include <ESP8266WiFi.h>

/* helpers */
bool isPressed(int button) { return digitalRead(button) == LOW; }
void setLed(bool on) { digitalWrite(LED_BUILTIN, on ? LOW : HIGH); }
void error(int errorCode) {
  while(true) {
    for(int i=0;i<errorCode*3;i++) {
      setLed(true);
      delay(250);
      setLed(false);
      delay(250);
    }
    delay(750);
  }
}