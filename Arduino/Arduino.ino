#include <WiFi.h>             //we need this for wifi stuff (duh)

void setup() {
  Serial.begin(115200);       //Set up Serial to allow us to print the nextline (the WiFiManager feedback) to the PC
  Serial.println("setup done with code '" + String(WiFiManager_Start()) + "'"); //"WiFiManager_Start()" will return a number, print this, see the WiFiManager tab for more info
}

void loop() {
 
}
