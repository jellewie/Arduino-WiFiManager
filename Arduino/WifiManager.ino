/* Written by JelleWho https://github.com/jellewie

   1. Load hardcoded data
   2. Load EEPROM data and save data
   3. while(no data) Set up AP mode and wait for user data
   4. try connecting, if (not) {GOTO3}
   5. if (AP mode used) {save to EEPROM}

   TODO
   give website feedback if error or something??
   Do EEPROM_SIZE check before writing (and ssid and password size check)

   DO NOT USE:
   Prefix 'WiFiManager' for anything
   Global variables of 'i' 'j' 'k' 'client'

   You can use "strip_ip, gateway_ip, subnet_mask" to set a static connection
*/

#define WiFiManager_ConnectionTimeOutMS 10000
#define WiFiManager_APSSID "ESP32"
#define WiFiManager_EEPROM_SIZE 64    //Max Amount of chars of 'SSID+PASSWORD' (+1)
#define WiFiManager_EEPROM_Seperator char(9)  //use 'TAB' as a seperator 
#define WiFiManager_LED LED_BUILTIN

//#define strip_ip, gateway_ip, subnet_mask to use static IP

#ifndef ssid
char ssid[WiFiManager_EEPROM_SIZE / 2] = "";
#endif //ssid

#ifndef password
char password[WiFiManager_EEPROM_SIZE / 2] = "";
#endif //password

#include <EEPROM.h>

byte WiFiManager_Start() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(WiFiManager_LED, HIGH);
  WiFiManager_ClearEEPROM();
  //starts wifi stuff, only returns when connected. will create Acces Point when needed
  /* <Return> <meaning>
     2 Can't begin EEPROM
     3 Can't write [all] data to EEPROM
  */
  if (!EEPROM.begin(WiFiManager_EEPROM_SIZE))
    return 2;
  String WiFiManager_a = WiFiManager_LoadEEPROM();
  if (WiFiManager_a != String(WiFiManager_EEPROM_Seperator)) {    //If there is data in EEPROM
    int i = WiFiManager_a.indexOf(char(WiFiManager_EEPROM_Seperator));
    if (i > 0)
      WiFiManager_SaveSSIDPassword(WiFiManager_a.substring(0, i), WiFiManager_a.substring(i + 1));
  }
  bool WiFiManager_APModeUsed;
  bool WiFiManager_Connected = false;
  while (!WiFiManager_Connected) {
    if ((strlen(ssid) != 0 or strlen(password) != 0)) {
      if (WiFiManager_Connect(10000)) { //try to connected to ssid password
        WiFiManager_Connected = true;
      } else {
        ssid[0] = (char)0;                  //Clear these so we will enter AP mode (*just clearing first bit)
        password[0] = (char)0;              //Clear these so we will enter AP mode
      }
    } else {
      WiFiManager_APModeUsed = true;
      WiFiManager_APMode();                 //No good ssid or password, entering APmode
    }
  }
  if (WiFiManager_APModeUsed) {
    if (!WiFiManager_WriteEEPROM(String(ssid), String(password)))
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
bool WiFiManager_WriteEEPROM(String WiFiManager_TempSSID, String WiFiManager_TempPassword) {
  String StringToWrite = WiFiManager_TempSSID + WiFiManager_EEPROM_Seperator + WiFiManager_TempPassword + char(255);  //Save to mem: <SSID><Seperator><Password><emthy bit>
  //(we use a emthy bit to mark the end)
  if (StringToWrite.length() > WiFiManager_EEPROM_SIZE)   //If nog enough room in the EEPROM
    return false;                                         //Return false; not all data is stored
  for (int i = 0; i < StringToWrite.length(); i++)        //For each character to save
    EEPROM.write(i, (int)StringToWrite.charAt(i));        //Write it to the EEPROM
  EEPROM.commit();
  return true;
}
void WiFiManager_SaveSSIDPassword(String WiFiManager_TempSSID, String WiFiManager_TempPassword) {
  WiFiManager_TempSSID.toCharArray(ssid, WiFiManager_TempSSID.length() + 1);
  WiFiManager_TempPassword.toCharArray(password, WiFiManager_TempPassword.length() + 1);
}
byte WiFiManager_APMode() {
  //IP of AP = 192.168.4.1
  /* <Return> <meaning>
     2 soft-AP setup Failed
     3
  */

  if (!WiFi.softAP(WiFiManager_APSSID))
    return 2;
  WiFiServer WiFiManager_server(80);
  WiFiManager_server.begin();
  bool WiFiManager_Looping = true;
  while (WiFiManager_Looping) {
    WiFiManager_Blink(100);  //Let the LED blink to show we are not connected
    WiFiClient client = WiFiManager_server.available();
    if (client) {
      //Serial.println("New Client.");
      String WiFiManager_TempInput;
      while (client.connected()) {
        if (client.available()) {
          char WiFiManager_c = client.read();               //read char by char HTTP request
          if (WiFiManager_TempInput.length() < 100)
            WiFiManager_TempInput += WiFiManager_c;                     //store characters to string
          if (WiFiManager_c == '\n') {                      //if HTTP request has ended
            //Serial.println(WiFiManager_TempInput);          //print to serial monitor for debuging
            if (WiFiManager_TempInput.indexOf('?') >= 0) {  //don't send new page
              client.println("HTTP/1.1 204 JelleWie\r\n\r\n");

              //This parts cuts the string and feedbacks the inputs
              //for example 'GET /set?Var1=Hhh&Var2=Yesi HTTP/1.1'
              //            'GET /<command>?<VariableName>=<Value>[&<VariableName2>=<Value2>]'  (where [] is opt)
              //Caps gets ignored, do not use these in names: '=' '?' ' '  '&' We use them to seperate inputs
              //Serial.println("_full_" + WiFiManager_TempInput);
              int i = WiFiManager_TempInput.indexOf(" ");
              if (i > 0)
                WiFiManager_TempInput = WiFiManager_TempInput.substring(i + 1); //Remove prefix "GET " (including ' ')
              i = WiFiManager_TempInput.indexOf(" ");
              if (i > 0)
                WiFiManager_TempInput = WiFiManager_TempInput.substring(0, i);      //Remove suffix " HTTP/1.1"
              String WiFiManager_command;                               //We need this here, since we need to access it in this {}
              i = WiFiManager_TempInput.indexOf("?");
              if (i > 0) {
                WiFiManager_command = WiFiManager_TempInput.substring(1, i);        //Get command "set" (excluding '/' and '?')
                WiFiManager_TempInput = WiFiManager_TempInput.substring(i + 1);     //Remove command "/set?" (including '?')
              }
              if (WiFiManager_command == "set") {
                i = WiFiManager_TempInput.indexOf("=");
                String WiFiManager_Temp_SSID;
                String WiFiManager_Temp_Password;
                while (i > 0) {
                  int j = WiFiManager_TempInput.indexOf("&");
                  String k, l;
                  if (j > 0) {                                          //If there more commands
                    l = WiFiManager_TempInput.substring(0, i);            //Get variable name "Var1"
                    k = WiFiManager_TempInput.substring(i + 1, j);  //Get first value variable "Hhh"
                    WiFiManager_TempInput = WiFiManager_TempInput.substring(j);     //Remove first var  "Var1=Hhh&" (including '&')
                  } else {
                    l = WiFiManager_TempInput.substring(1, i);            //Get variable name "Var2"
                    k = WiFiManager_TempInput.substring(i + 1);           //Get second value variable "Yesi"
                    WiFiManager_TempInput = "";                           //No more commands, clear it so we stop
                  }
                  //Serial.println("'" + l + "'='" + k + "'");            //Debug all commands
                  l.toLowerCase();                                        //Convert to lowerCase so we are not CaPs SeNsAtIvE
                  if (l == "ssid")
                    WiFiManager_Temp_SSID = k;
                  else if (l == "password")
                    WiFiManager_Temp_Password = k;
                  i = WiFiManager_TempInput.indexOf("=");
                }
                //Serial.println("SSID='" + WiFiManager_Temp_SSID + "' Password='" + WiFiManager_Temp_Password + "'");    //Debug
                WiFiManager_SaveSSIDPassword(WiFiManager_Temp_SSID, WiFiManager_Temp_Password);
                WiFiManager_Looping = false;
              } else {
                //Serial.println("Unknown command '" + WiFiManager_command + "'");
              }
            }
            else {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line:
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();

              client.print("<div>ESPNAME</div>");
              client.print("<form action=\"/set?a=1&amp;b=2\" method=\"get\">");
              client.print("<div><label for=\"SSID\">SSID</label> <input name=\"SSID\" type=\"text\" value=\"\" /></div>");
              client.print("<div><label for=\"Password\">Password</label> <input name=\"Password\" type=\"text\" value=\"\" /></div>");
              client.print("<div><button>Send</button></div>");
              client.print("</form>");
            }
            delay(1);
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
