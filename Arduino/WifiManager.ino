/* Written by JelleWho https://github.com/jellewie

   1. Load hardcoded data
   2. Load EEPROM data and save data
   3. while(no data) Set up AP mode and wait for user data
   4. try connecting, if (not) {GOTO3}
   5. if (AP mode used) {save to EEPROM}

   TODO
   give website feedback if error or something??

   DO NOT USE:
   Prefix 'WiFiManager' for anything
   Global variables of 'i' 'j' 'k' 'client'

   You can use "strip_ip, gateway_ip, subnet_mask" to set a static connection

   HOW TO ADD CUSTOM CALUES
   -"WiFiManager_VariableNames" Add the 'AP settings portal' name
   -"WiFiManager_EEPROM_SIZE"   [optional] Make sure it is big enough; if it's 10 and you have 2 settings (SSID, Password) than both get 10/2=5 bytes of storage, probably not enough (1 byte = 1 character)
   -"WiFiManager_Set_Value"     Set the action on what to do on startup with this value
   -"WiFiManager_Get_Value"     [optional] Set the action on what to fill in in the boxes in the 'AP settings portal'
*/

#define WiFiManager_ConnectionTimeOutMS 10000
#define WiFiManager_APSSID "ESP32"
#define WiFiManager_EEPROM_SIZE 64    //Max Amount of chars of 'SSID+PASSWORD' (+1) (+extra custom vars)
#define WiFiManager_EEPROM_Seperator char(9)  //use 'TAB' as a seperator 
#define WiFiManager_LED LED_BUILTIN

const String WiFiManager_VariableNames[] {"SSID", "Password"};
const static byte WiFiManager_Settings = sizeof(WiFiManager_VariableNames) / sizeof(WiFiManager_VariableNames[0]); //Why filling this in if we can automate that? :)
const static byte WiFiManager_EEPROM_SIZE_EACH = WiFiManager_EEPROM_SIZE / WiFiManager_Settings;

//#define strip_ip, gateway_ip, subnet_mask to use static IP

#ifndef ssid
char ssid[WiFiManager_EEPROM_SIZE_EACH] = "test";
#endif //ssid

#ifndef password
char password[WiFiManager_EEPROM_SIZE_EACH] = "";
#endif //password

#include <EEPROM.h>

byte WiFiManager_Start() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(WiFiManager_LED, HIGH);
  //starts wifi stuff, only returns when connected. will create Acces Point when needed
  /* <Return> <meaning>
     2 Can't begin EEPROM
     3 Can't write [all] data to EEPROM
  */
  if (!EEPROM.begin(WiFiManager_EEPROM_SIZE))
    return 2;
  String WiFiManager_a = WiFiManager_LoadEEPROM();
  //Serial.println("EEPROM data=" + WiFiManager_a);
  if (WiFiManager_a != String(WiFiManager_EEPROM_Seperator)) {    //If there is data in EEPROM
    for (byte i = 0; i < WiFiManager_Settings; i++) {
      byte j = WiFiManager_a.indexOf(char(WiFiManager_EEPROM_Seperator));
      if (j == 255)
        j = WiFiManager_a.length();
      String WiFiManager_TEMP_Value = WiFiManager_a.substring(0, j);
      if (WiFiManager_TEMP_Value != "")                     //If there is a value
        WiFiManager_Set_Value(i, WiFiManager_TEMP_Value);   //set the value in memory (and thus overwrite the Hardcoded stuff)
      WiFiManager_a = WiFiManager_a.substring(j + 1);
    }
  }
  bool WiFiManager_APModeUsed = false;
  bool WiFiManager_Connected = false;
  while (!WiFiManager_Connected) {
    if ((strlen(ssid) == 0 or strlen(password) == 0)) {
      WiFiManager_APModeUsed = true;
      WiFiManager_APMode();                 //No good ssid or password, entering APmode
    } else {
      //Serial.println("Connect to " + String(ssid) + " and " + String(password));
      if (WiFiManager_Connect(10000))     //try to connected to ssid password
        WiFiManager_Connected = true;
      else
        password[0] = (char)0;              //Clear this so we will enter AP mode (*just clearing first bit)
    }
  }
  if (WiFiManager_APModeUsed) {
    if (!WiFiManager_WriteEEPROM())
      return 3;
  }
  digitalWrite(WiFiManager_LED, LOW);
  return 1;
}
String WiFiManager_LoadEEPROM() {
  String WiFiManager_Value;
  for (int i = 0; i < WiFiManager_EEPROM_SIZE; i++) {
    byte WiFiManager_Input = EEPROM.read(i);
    if (WiFiManager_Input == 255)               //If at the end of data
      return WiFiManager_Value;                 //Stop and return all data stored
    if (WiFiManager_Input == 0)                 //If no data found (NULL)
      return String(WiFiManager_EEPROM_Seperator);
    WiFiManager_Value += char(WiFiManager_Input);
    //Serial.println(String(i) + "=" + char(WiFiManager_Input));
  }
  return String(WiFiManager_EEPROM_Seperator);    //ERROR; [maybe] not enough space
}
bool WiFiManager_WriteEEPROM() {
  String StringToWrite;                               //Save to mem: <SSID>
  for (byte i = 0; i < WiFiManager_Settings; i++) {
    StringToWrite += WiFiManager_Get_Value(i, true); //^            <Seperator>
    if (WiFiManager_Settings - i > 1)
      StringToWrite += WiFiManager_EEPROM_Seperator;  //^            <Value>  (only if there more values)
  }
  StringToWrite += char(255);                         //^            <emthy bit> (we use a emthy bit to mark the end)
  if (StringToWrite.length() > WiFiManager_EEPROM_SIZE)   //If nog enough room in the EEPROM
    return false;                                         //Return false; not all data is stored
  for (int i = 0; i < StringToWrite.length(); i++)        //For each character to save
    EEPROM.write(i, (int)StringToWrite.charAt(i));        //Write it to the EEPROM
  EEPROM.commit();
  return true;
}
byte WiFiManager_APMode() {
  //IP of AP = 192.168.4.1
  /* <Return> <meaning>
     2 soft-AP setup Failed
     3
  */
  //Serial.println("APMode SETTING UP");
  if (!WiFi.softAP(WiFiManager_APSSID))
    return 2;
  WiFiServer WiFiManager_server(80);
  WiFiManager_server.begin();
  bool WiFiManager_Looping = true;
  //Serial.println("APMode ON");
  while (WiFiManager_Looping) {
    WiFiManager_Blink(100);  //Let the LED blink to show we are not connected
    WiFiClient client = WiFiManager_server.available();
    if (client) {
      //Serial.println("New Client.");
      String WiFiManager_Temp_Input;
      while (client.connected()) {
        if (client.available()) {
          char WiFiManager_c = client.read();               //read char by char HTTP request
          if (WiFiManager_Temp_Input.length() < 100)
            WiFiManager_Temp_Input += WiFiManager_c;                     //store characters to string
          if (WiFiManager_c == '\n') {                      //if HTTP request has ended
            //Serial.println(WiFiManager_Temp_Input);          //print to serial monitor for debuging
            if (WiFiManager_Temp_Input.indexOf('?') >= 0) {  //don't send new page
              client.println("HTTP/1.1 204 JelleWie\r\n\r\n");

              //This parts cuts the string and feedbacks the inputs
              //for example 'GET /set?Var1=Hhh&Var2=Yesi HTTP/1.1'
              //            'GET /<command>?<VariableID>=<Value>[&<VariableID2>=<Value2>]'  (where [] is opt)
              //Caps gets ignored, do not use these in names: '=' '?' ' '  '&' We use them to seperate inputs
              //Serial.println("_full_" + WiFiManager_Temp_Input);

              int i = WiFiManager_Temp_Input.indexOf(" ");
              if (i > 0)
                WiFiManager_Temp_Input = WiFiManager_Temp_Input.substring(i + 1); //Remove prefix "GET " (including ' ')
              i = WiFiManager_Temp_Input.indexOf(" ");
              if (i > 0)
                WiFiManager_Temp_Input = WiFiManager_Temp_Input.substring(0, i);  //Remove suffix " HTTP/1.1"
              String WiFiManager_command;                                         //We need this here, since we need to access it in this {}
              i = WiFiManager_Temp_Input.indexOf("?");
              if (i > 0) {
                WiFiManager_command = WiFiManager_Temp_Input.substring(1, i);     //Get command "set" (excluding '/' and '?')
                WiFiManager_Temp_Input = WiFiManager_Temp_Input.substring(i + 1); //Remove command "/set?" (including '?')
              }

              if (WiFiManager_command == "set") {
                for (byte i = 0; i < WiFiManager_Settings; i++) {
                  byte j = WiFiManager_Temp_Input.indexOf("&");
                  if (j == 255)
                    j = WiFiManager_Temp_Input.length();
                  byte k = WiFiManager_Temp_Input.indexOf("=");
                  if (k < 255)
                    WiFiManager_Set_Value(WiFiManager_Temp_Input.substring(0, k).toInt(), WiFiManager_Temp_Input.substring(k + 1, j));
                  WiFiManager_Temp_Input = WiFiManager_Temp_Input.substring(j + 1); //Remove command
                }
                WiFiManager_Looping = false;
              } else {
                //Serial.println("Unknown command '" + WiFiManager_command + "'");
              }
            } else {
              client.println("HTTP/1.1 200 OK");        //HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              client.println("Content-type:text/html"); //And a content-type so the client knows what's coming
              client.println("Connection: close");      //
              client.println();                         //Then a blank line

              client.print("<div>ESP32 settings</div><form action=\"/set?a=1&amp;b=2\" method=\"get\">");
              for (byte i = 0; i < WiFiManager_Settings; i++)
                client.print("<div><label for=\" " + WiFiManager_VariableNames[i] + "\"> " + WiFiManager_VariableNames[i] + "</label> <input name=\"" + i + "\" type=\"text\" value=\"" + WiFiManager_Get_Value(i, false) + "\" /></div>");
              client.print("<div><button>Send</button></div></form>");
            }
            client.stop();
          }
        }
      }
      //Serial.println("Lost Client.");
    }
  }
  //Serial.println("AP Done");
  return 1;
}
bool WiFiManager_Connect(int WiFiManager_TimeOutMS) {
  //Serial.println("Connect to ssid='" + String(ssid) + "' password='" + String(password) + "'");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
#if defined(strip_ip) && defined(gateway_ip) && defined(subnet_mask)
  WiFi.config(strip_ip, gateway_ip, subnet_mask);
#endif
  unsigned long WiFiManager_StopTime = millis() + WiFiManager_TimeOutMS;
  while (WiFi.status() != WL_CONNECTED) {
    if (WiFiManager_StopTime < millis())     //If we are in overtime
      return false;
    WiFiManager_Blink(500);  //Let the LED blink to show we are not connected
  }
  return true;
}
void WiFiManager_Blink(int WiFiManager_Temp_Delay) {
  static unsigned long LastTime = millis();
  if (millis() > LastTime + WiFiManager_Temp_Delay) {
    LastTime = millis();
    digitalWrite(WiFiManager_LED, !digitalRead(WiFiManager_LED));
  }
}
void WiFiManager_Set_Value(byte WiFiManager_ValueID, String WiFiManager_Temp) {
  //Serial.println("Set value " + String(WiFiManager_ValueID) + " = " + WiFiManager_Temp);
  WiFiManager_Temp.trim();                  //remove leading and trailing whitespace
  switch (WiFiManager_ValueID) {
    case 0:
      WiFiManager_Temp.toCharArray(ssid, WiFiManager_Temp.length() + 1);
      break;
    case 1:
      WiFiManager_Temp.toCharArray(password, WiFiManager_Temp.length() + 1);
      break;
  }
}
String WiFiManager_Get_Value(byte WiFiManager_ValueID, bool WiFiManager_Safe) {
  //Serial.println("Get value " + String(WiFiManager_ValueID));
  String WiFiManager_Temp_Return = "";                //Make sure to return something, if we return bad data of NULL, the HTML page will break
  switch (WiFiManager_ValueID) {
    case 0:
      WiFiManager_Temp_Return += String(ssid);
      break;
    case 1:
      if (WiFiManager_Safe)                           //If's it's safe to return password.
        WiFiManager_Temp_Return += String(password);
      break;
  }
  return String(WiFiManager_Temp_Return);      
}
//Some debug functions
//void WiFiManager_ClearEEPROM() {
//  EEPROM.write(0, 0);   //We just need to clear the first one, this will spare the EEPROM write cycles and would work fine
//  EEPROM.commit();
//}
//void WiFiManager_ClearMEM() {
//  ssid[0] = (char)0;                  //Clear these so we will enter AP mode (*just clearing first bit)
//  password[0] = (char)0;              //Clear these so we will enter AP mode
//}
//String WiFiManager_return_ssid() {
//  return ssid;
//}
//String WiFiManager_return_password() {
//  return password;
//}
