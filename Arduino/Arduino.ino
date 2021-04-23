/*
  Program written by JelleWho https://github.com/jellewie
  Board: https://dl.espressif.com/dl/package_esp32_index.json
*/

#if !defined(ESP32)
#error "Please check if the 'DOIT ESP32 DEVKIT V1' board is selected, which can be downloaded at https://dl.espressif.com/dl/package_esp32_index.json"
#endif

#include "WiFiManagerBefore.h"                                  //Define what options to use/include or to hook into WiFiManager
#include "WiFiManager/WiFiManager.h"                            //Includes <WiFi> and <WebServer.h> and setups up 'WebServer server(80)' if needed
#include "WiFiManagerLater.h"                                   //Define options of WiFiManager (can also be done before), but WiFiManager can also be called here (example for DoRequest included here)
																  
void setup() {                                                    
  Serial.begin(115200);                                           
  //server.on("/url", CodeToCall);                              //Example of a url to function caller, These must be declaired before "WiFiManager.Start()"
  byte Answer = WiFiManager.Start();                            //Run the wifi startup (and save results)
  Serial.println("WiFi setup executed with responce code '" + String(Answer) + "'"); //The return codes can be found in "WiFiManager.cpp" in "CWiFiManager::Start("
}
void loop() {
  //WiFiManager.RunServer();                                    //Do WIFI server stuff if needed. Only uncomment this if you need the server
}
