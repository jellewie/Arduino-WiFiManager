/* Written by JelleWho https://github.com/jellewie
   https://github.com/jellewie/Arduino-WiFiManager

  These are some examples how to 'hook' functions with code into the WiFiManager.
  This file and all functions are not required, but when defined before '#include "WiFiManagerUser.h"' they will be hooked in to WiFiManager
  if you want to declair functions after including WiFiManager, uses a 'extern <function name>' to declair a dummy here, so the hook will be made, but will be hooked to a later (extern if I may say) declaration

  NOTES
   DO NOT USE char(") in any of input stings on the webpage, use char(') if you need it. char(") will be replaced

  HOW TO ADD CUSTOM VALUES
   -"WiFiManagerUser_VariableNames_Defined" define this, and ass custom names for the values
   -"WiFiManager_VariableNames"  Add the description name of the value to get/set to this list
   -"EEPROM_size"     [optional] Make sure it is big enough for your needs, SIZE_SSID+SIZE_PASS+YourValues (1 byte = 1 character)
   -"Set_Value"       Set the action on what to do on startup with this value
   -"Get_Value"       [optional] Set the action on what to fill in in the boxes in the 'AP settings portal'
*/
//===========================================================================
// Things that need to be defined before including "WiFiManager.h"
//===========================================================================
#define WiFiManager_SerialEnabled                               //Define to send Serial debug feedback

#define WiFiManagerUser_Set_Value_Defined                       //Define we want to hook into WiFiManager
#define WiFiManagerUser_Get_Value_Defined                       //^
#define WiFiManagerUser_Status_Start_Defined                    //^
#define WiFiManagerUser_Status_Done_Defined                     //^
#define WiFiManagerUser_Status_Blink_Defined                    //^
#define WiFiManagerUser_Status_StartAP_Defined                  //^
#define WiFiManagerUser_HandleAP_Defined                        //^

//#define WiFiManagerUser_VariableNames_Defined                 //Define that we want to use the custom user variables (Dont forget to settup WiFiManager_VariableNames and WiFiManager_Settings)
//const String WiFiManager_VariableNames[] = {"SSID", "Password", "Custom variable Name 1"};
//const byte WiFiManager_Settings = sizeof(WiFiManager_VariableNames) / sizeof(WiFiManager_VariableNames[0]); //Why filling this in if we can automate that? :)

//const byte EEPROM_size = 255;                                 //Max Amount of chars for 'SSID(16) + PASSWORD(16) + extra custom vars(?) +1(NULL)' defaults to 33

//#define WiFiManagerUser_APSSID_Defined
//char APSSID[16] = "ESP32";                                    //If you want to define the name somewhere else use 'char* APSSID = Name'

//===========================================================================
const byte Pin_LED  = LED_BUILTIN;                              //Just here for some examples, It's the LED to give feedback on (like blink on error)
//===========================================================================
bool WiFiManagerUser_Set_Value(byte ValueID, String Value) {
  switch (ValueID) {                                            //Note the numbers are shifted from what is in memory, 0 is the first user value
    case 0: {
        String SomeString = Value;
        return true;                                            //Report back a succesfull set
      } break;
    case 1: {
        int SomeValue = Value.toInt();
        return true;
      } break;
  }
  return false;                                                 //Report back that the ValueID is unknown, and we could not set it
}
//===========================================================================
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
//===========================================================================
void WiFiManagerUser_Status_Start() { //Called before start of WiFi
  pinMode(Pin_LED, OUTPUT);
  digitalWrite(Pin_LED, HIGH);
}
//===========================================================================
void WiFiManagerUser_Status_Done() { //Called after succesfull connection to WiFi
  digitalWrite(Pin_LED, LOW);
}
//===========================================================================
void WiFiManagerUser_Status_Blink() { //Used when trying to connect/not connected
  digitalWrite(Pin_LED, !digitalRead(Pin_LED));
}
//===========================================================================
void WiFiManagerUser_Status_StartAP() {}
//===========================================================================
bool WiFiManagerUser_HandleAP() {                               //Called when in the While loop in APMode, this so you can exit it
  //Return true to leave APmode
#define TimeOutApMode 15 * 60 * 1000;                           //Example for a timeout, (time in ms)
  unsigned long StopApAt = millis() + TimeOutApMode;
  if (millis() > StopApAt)    return true;                      //If we are running for to long, then flag we need to exit APMode
  return false;
}
