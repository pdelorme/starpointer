// Replace with your network credentials
const char* SP_ssid = "STAR_POINTER";
const char* SP_password = "1234567890";

// const char* HOME_ssid = "SFR_DELDEL";
const char* HOME_ssid = "PATER_LAB";
const char* HOME_password = "####";

void wifiClient(){
  // Connect to Wi-Fi
  WiFi.begin(HOME_ssid, HOME_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
}

void wifiAccessPoint(){
  WiFi.softAP(SP_ssid, SP_password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
}

void setupWifi(){
    //wifiAccessPoint();
    wifiClient();
}
