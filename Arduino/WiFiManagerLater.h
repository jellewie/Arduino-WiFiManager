/* Written by JelleWho https://github.com/jellewie
   https://github.com/jellewie/Arduino-WiFiManager
 */
//===========================================================================
// Things that can/need to be defined after including "WiFiManager.h"
//===========================================================================
const byte Pin_LED  = LED_BUILTIN;                              //Just here for some examples, It's the LED to give feedback on (like blink on error)
bool WiFiManagerUser_Set_Value(byte ValueID, String Value) {
  switch (ValueID) {                                            //Note the numbers are shifted from what is in memory, 0 is the first user value
    case 0: {
        String SomeString = Value;
        return true;                                            //Report back a succesfull set
      } break;
    case 1: {
        int SomeValue = Value.toInt();
        SomeValue = SomeValue + 0;                              //Just here to 'use' the value so the compiler wont warn us about it
        return true;
      } break;
  }
  return false;                                                 //Report back that the ValueID is unknown, and we could not set it
}
String WiFiManagerUser_Get_Value(byte ValueID, bool Safe, bool Convert) {
  //if its 'Safe' to return the real value (for example the password will return '****' or '1234')
  //'Convert' the value to a readable string for the user (bool '0/1' to 'FALSE/TRUE')
  switch (ValueID) {                                            //Note the numbers are shifted from what is in memory, 0 is the first user value
    case 0: {
        bool SomeBool = true;
        if (Convert)
          return SomeBool ? "FALSE" : "TRUE";                   //Convert bool to readable string
        else
          return String(SomeBool);                              //Just return the boolean
      } break;
    case 1:
      return "Some text";
      break;
  }
  return "";
}
void WiFiManagerUser_Status_Start() {                           //Called before start of WiFi
  pinMode(Pin_LED, OUTPUT);
  digitalWrite(Pin_LED, HIGH);
}
void WiFiManagerUser_Status_Done() {                            //Called after succesfull connection to WiFi
  digitalWrite(Pin_LED, LOW);
}
void WiFiManagerUser_Status_Blink() {                           //Used when trying to connect/not connected
  digitalWrite(Pin_LED, !digitalRead(Pin_LED));
}
void WiFiManagerUser_Status_StartAP() {}                        //Called before start of APmode
bool WiFiManagerUser_HandleAP() {                               //Called when in the While loop in APMode, this so you can exit it
  //Return true to leave APmode
#define TimeOutApMode 15 * 60 * 1000;                           //Example for a timeout, (time in ms)
  unsigned long StopApAt = millis() + TimeOutApMode;
  if (millis() > StopApAt)    return true;                      //If we are running for to long, then flag we need to exit APMode
  return false;
}
void ExampleDoRequest(){
  char IP[16] = "192.168.255.255";                              //The url to connect to
  int Port = 80;                                                //the port to connect to, Defaults to 80
  String Path = "/json.htm";                                    //The url path to go to, Defaults to emthy ''
  String Json = "{'m':1}";                                      //Json content to send, Defaults to emthy ''
  int Timeout = 1000;                                           //Time in ms for a timeout, Defaults to 1000ms
  //DO REQUEST: IP:Port&Path (with data) Json
  
  byte Answer = WiFiManager.DoRequest(IP, Port, Path, Json, Timeout);
  Serial.println("DoRequest executed with responce code '" + String(Answer) + "'"); //The return codes can be found in "WiFiManager.cpp" in "CWiFiManager::DoRequest("
}
