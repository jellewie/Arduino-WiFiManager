/*
  Program written by JelleWho
  Board: https://dl.espressif.com/dl/package_esp32_index.json
*/
#include "WiFiManagerUser.h"          //Define custon functions to hook into WiFiManager
#include "WiFiManager/WiFiManager.h"  //Includes <WiFi> and <WebServer.h> and setups up 'WebServer server(80)' if needed

void setup() {
  Serial.begin(115200);
  //server.on("/url", CodeToCall);    //Example of a url to function caller, These must be declaired before "WiFiManager.Start()"
  //WiFiManager.StartServer();        //Start the server (if you need this for other stuff)
  
  byte Answer = WiFiManager.Start();                      //run the wifi startup (and save results)
  Serial.println("setup done with code '" + String(Answer) + "'");
  
  //WiFiManager.EnableSetup(true);    //Enable the settings, only enabled in APmode by default
  //WiFiManager.WriteEEPROM();        //Since we can't really check if the new input values work, this is just stored local and not in EEPROM
                                      //^use this to save values to (EEPROM LIMITED WRITES AVAILIBLE! do not spam)
}
void loop() {

  //WiFiManager.RunServer();          //Do WIFI server stuff if needed. Only uncomment this if you need the server. You must have started the server before this with 'WiFiManager_StartServer()'
}
