/*
  Program written by JelleWho https://github.com/jellewie
  Board: https://dl.espressif.com/dl/package_esp32_index.json
*/
#include "WiFiManagerUser.h"          //Define custon functions to hook into WiFiManager
#include "WiFiManager/WiFiManager.h"  //Includes <WiFi> and <WebServer.h> and setups up 'WebServer server(80)' if needed

void setup() {
  Serial.begin(115200);
  //server.on("/url", CodeToCall);    //Example of a url to function caller, These must be declaired before "WiFiManager.Start()"
  byte Answer = WiFiManager.Start();                      //run the wifi startup (and save results)
  Serial.println("setup done with code '" + String(Answer) + "'");
}
void loop() {
  //WiFiManager.RunServer();          //Do WIFI server stuff if needed. Only uncomment this if you need the server. You must have started the server before this with 'WiFiManager_StartServer()', Also required for OTA and such
}
