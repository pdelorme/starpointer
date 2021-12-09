Sgp4 sat;

char satname[] = "ISS (ZARYA)";
char tle_line1[] = "1 25544U 98067A 21324.06457731 .00001146 00000+0 29342-4 0 9990";  //Line one from the TLE data
char tle_line2[] = "2 25544 51.6442 291.9657 0004633 219.6337 260.2444 15.48598792 312758";  //Line two from the TLE data
int tle_line1_index = 0;
int tle_line2_index = 70;

int sign(double value){
  if(value<0) return -1;
  return 1;
}

double julianDate(){
  double jday = 367.*year() - rint((7.*(year() + rint((month() + 9.)/12.)))/4.) + rint((275.*month())/9.);
  jday += (day() + 1721013.5 - 0.5 * sign(100*year() + month() - 190002.5) + 0.5);
  return jday;
}

boolean fetchLTE(){
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  Serial.println("Fetching ISS DATA from https://tle.ivanstanojevic.me/api/tle/25544...");
  if(http.begin(client, "https://tle.ivanstanojevic.me/api/tle/25544")){
    // http.addHeader("Content-Type", "application/json");
    int httpCode = http.GET();
    if (httpCode > 0) {
        Serial.println(http.getString());
        const size_t bufferSize = JSON_OBJECT_SIZE(8);
        StaticJsonDocument<bufferSize> root;
        DeserializationError error = deserializeJson(root, http.getString());
        String jsonString;
        serializeJson(root, jsonString);
        Serial.println(jsonString);
        String name = root["name"];
        String line1 = root["line1"];
        Serial.println(line1);
        String line2 = root["line2"];
        Serial.println(line2);
        name.reserve(10);
        name.toCharArray(satname,10);
        line1.toCharArray(tle_line1,70);
        line2.toCharArray(tle_line2,70);
        Serial.println(String(tle_line1));
        return true;
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  } else {
    Serial.printf("[HTTP] Unable to connect\n");
  }
  return false;
}

void setupSatTrack(){
  //1. init location on earth
  sat.site(locationOnEarth.geoCoords.lat,locationOnEarth.geoCoords.lon,locationOnEarth.alt); //set site latitude[°], longitude[°] and altitude[m]
  //2. get latests TLE if we are connected to the internet
  if(fetchLTE()){
    // persist to eeprom
    Serial.println("Saving tle to EEPROM");
    writeStringToEEPROM(tle_line1_index, tle_line1);
    writeStringToEEPROM(tle_line2_index, tle_line2);
  } else {
    // load from eeprom
    readCharArrayFromEEPROM(tle_line1_index, tle_line1, 70);
    readCharArrayFromEEPROM(tle_line2_index, tle_line2, 70);
    Serial.println("Reading tle from EEPROM");
    Serial.println(String(tle_line1));
  }
  sat.init(satname,tle_line1,tle_line2);     //initialize satellite parameters 
}

void loopSatTrack(){
  if(targetType == ISSTarget  && (millis()-updateMillis>1000)){
    updateMillis = millis();
    sat.findsat(julianDate());
    goToHCoords(sat.satEl, sat.satAz);
  }
}
