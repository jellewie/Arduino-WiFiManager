# Arduino-WiFiManager
 ESP32 WiFiManager sketch

# How it works
Call "WiFiManager_Start()" to start, this will:
1) Load hardcoded WIFI data (already there on boot)
2) Load EEPROM WIFI data and save data 
3) while (no data) Set up AP mode and wait for user data
4) try connecting, if (not) {GOTO3}
5) if (AP mode used) {save to EEPROM}

So basically, on boot it tries to connect to the WIFI network, and if not possible it would create a Access Point so you can set the WIFI settings.

# How to set up the WIFI settings
there are a few ways to do this
1) Hardcoded; set 'ssid' and 'password' in the WiFiManager file. (this will be overwritten by EEPROM, if this has newer data)
2) AP mode; When no Hardcoded or EEPROM data available the ESP will start a WIFI Acces Point, steps to set up this info:
-Connect to "ESP32" (WiFiManager_APSSID)
-Goto "196.168.4.1"
-Fill in WIFI settings, and press SEND. (Data will be saved to EEPROM when it works)