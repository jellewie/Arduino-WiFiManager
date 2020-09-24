/*
  Program written by JelleWho
  Board: https://dl.espressif.com/dl/package_esp32_index.json
*/
#include <WiFi.h>                   //we need this for wifi stuff (duh)
#include <WebServer.h>
WebServer server(80);
#include "WifiManager.h"

void setup() {
  Serial.begin(115200);
  server.on("/",          WiFiManager_handle_Connect);    //Must be declaired before "WiFiManager_Start()" for APMode
  server.on("/setup",     WiFiManager_handle_Settings);   //Must be declaired before "WiFiManager_Start()" for APMode

  byte Answer = WiFiManager.Start();                      //run the wifi startup (and save results)
  
  Serial.println("setup done with code '" + String(Answer) + "'");
  
  //WiFiManager.StartServer();      //Start the server (if you need this for other stuff)
  //WiFiManager.EnableSetup(true);  //Enable the settings, only enabled in APmode by default
  //WiFiManager.WriteEEPROM();      //Since we can't really check if the new input values work, this is just stored local and not in EEPROM
                                    //^use this to save values to (EEPROM LIMITED WRITES AVAILIBLE! do not spam)
}
void loop() {

  //WiFiManager.RunServer();     //Do WIFI server stuff if needed. Only uncomment this if you need the server. You must have started the server before this with 'WiFiManager_StartServer()'
}
