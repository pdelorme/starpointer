// Replace with your network credentials
char* SP_ssid = "STAR_POINTER";
char* SP_password = "1234567890";

/*
 * try to connect to local wifi.
 * fails after 10s and set SP access point.
 * return true if connected.
 */
boolean connectToLocalWifiString(String ssid, String password){
  char ssid_c[32], password_c[64];
  ssid.toCharArray(ssid_c, 32);
  password.toCharArray(password_c, 64);
  return connectToLocalWifi(ssid_c, password_c);
}

boolean connectToLocalWifi(char* ssid, char* password){
  debugln("Connecting to Local Wifi : "+String(ssid)+" / "+String(password));
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(attempts++>20) {
      Serial.println();
      Serial.println("FAILED Connecting to Local Wifi !");
      wifiAccessPoint();
      return false;
    }
    debug(".+");
    delay(1000);
  }
  Serial.println("Connected to Local Wifi !");
  // Print ESP8266 Local IP Address
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);
  // save to EEPROM
  writeStringToEEPROM(EEPROM_SSID_OFFSET, String(ssid));
  writeStringToEEPROM(EEPROM_PWD_OFFSET, String(password));
  writeStringToEEPROM(EEPROM_IP_OFFSET, ip.toString()); 
  // fetching ISS Data
  // fetchLTE();
  return true;
}

void wifiAccessPoint(){
  Serial.println("SettingUp AccessPoint");
  WiFi.softAP(SP_ssid, SP_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
}

void setupWifi(){
  if(readBooleanFromEEPROM(EEPROM_WIFI_OFFSET)){
    String ssid = readStringFromEEPROM(EEPROM_SSID_OFFSET);
    String password = readStringFromEEPROM(EEPROM_PWD_OFFSET);
    debug("Using EEPROM");
    connectToLocalWifiString(ssid, password);
  } else {
    wifiAccessPoint();
  }
}

/**
 * Unit Testing.
 */
#if TEST == 1

test(connectToLocalWifi){
  boolean connected = connectToLocalWifi(HOME_SSID, HOME_PWD);
  assertTrue(connected);
}

test(connectToBadLocalWifi){
  boolean connected = connectToLocalWifi("yoyo","vladivostok");
  assertFalse(connected);
}

test(wifiAccessPointThenConnectToLocalWifi){
  wifiAccessPoint();
  boolean connected = connectToLocalWifi(HOME_SSID, HOME_PWD);
  assertTrue(connected);
}
#endif
