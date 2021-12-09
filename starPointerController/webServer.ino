// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
const char* PARAM_COMMAND = "command";

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  String cmd, val;
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      data[len] = 0;
      Serial.printf("got some data");
      Serial.printf((char*)data);
      Serial.println();
      cmd = String((char*)data).substring(0,4);
      val = String((char*)data).substring(5);
      Serial.println(cmd);
      Serial.println(val);
      if(cmd == "TIME"){
        int yr, mnth, d, h, m, s;
        sscanf( (char*)data, "TIME:%4d-%2d-%2dT%2d:%2d:%2dZ", &yr, &mnth, &d, &h, &m, &s);
        setTime(h,m,s,d,mnth,yr);
      } else if(cmd == "GPS_"){
        float lat, lon;
        sscanf( (char*)data, "LAT=%f,LON=%f", &lat, &lon);
        setLocationOnEarth(lat, lon);
      } else if(cmd == "LOC_"){
        setLocationOnEarth(getLocation(val));
      } else if(cmd == "SETP"){
        setLaserPositionFromPolaris();
      } else if(cmd == "MOVE"){
        if (val.startsWith("UP")){ up(1);    } 
        else if (val == "DOWN")  { down(1);  }
        else if (val == "LEFT")  { left(1);  }
        else if (val == "RIGHT") { right(1); }
      } else if(cmd == "GOTO"){
        targetName=val;
        if (val == "NORTH") {
          goToHCoords(targetHCoord.alt,0);
        } else if (val == "SOUTH"){
          goToHCoords(targetHCoord.alt,180);
        } else if (val == "WEST") {
          goToHCoords(targetHCoord.alt,270);
        } else if (val == "EAST"){
          goToHCoords(targetHCoord.alt,90);
        } else if (val == "HORIZON"){
          goToHCoords(0,targetHCoord.azi);
        } else if (val == "ZENITH"){
          goToHCoords(90,targetHCoord.azi);
        } else if (val == "POLARIS") { 
          solarSystemObjectIndex = Earth;
          goToEqCoords(polarisEqCoord); 
        } else if (val == "ISS") { 
          targetName = satname;
          targetType = ISSTarget;
          setupSatTrack();
        } else if (val.startsWith("S")) {
          SolarSystemRef target = getSolarObject(val);
          targetName = target.name;
          targetType = SolarSystemTarget;
          solarSystemObjectIndex = target.index;
        } else if (val.startsWith("M")) {
          MessierObject target = getMessierObject(val);
          targetName = target.commonName;
          targetType = StellarTarget;
          goToEqCoords(target.eqCoord);
        }
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}
void setupWebserver(){
  // Launch SPIFFS file system | Démarre le système de fichier SPIFFS 
  if(!SPIFFS.begin()){ 
    Serial.println("An Error has occurred while mounting SPIFFS");  
  }

  // static file server
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  server.on("/setup", HTTP_POST, [] (AsyncWebServerRequest *request) {
    int params = request->params();
    for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        Serial.print(p->name().c_str());
        Serial.print(p->value().c_str());
    }
  });

  // Simple INFO
  server.on("/info", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "HELLO V1");
  });

  server.on("/solarObject.json", HTTP_GET, [](AsyncWebServerRequest *request){
    String code;
    if (request->hasParam("code")) {
      code = request->getParam("code")->value();
    }
    else {
      request->send(400);
      return;
    }
    SolarSystemRef solarSystemRef = getSolarObject(code);
    const size_t CAPACITY = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<CAPACITY> doc;
    JsonObject root = doc.to<JsonObject>();
    root["code"] = solarSystemRef.code;
    root["label"] = solarSystemRef.name;    
    String jsonString;
    serializeJson(doc, jsonString);
    request->send(200, "application/json", jsonString);
  });

  server.on("/messierObject.json", HTTP_GET, [](AsyncWebServerRequest *request){
    String code;
    if (request->hasParam("code")) {
      code = request->getParam("code")->value();
    }
    else {
      request->send(400);
      return;
    }
    MessierObject messierObject = getMessierObject(code);
    const size_t CAPACITY = JSON_OBJECT_SIZE(10);
    StaticJsonDocument<CAPACITY> doc;
    JsonObject root = doc.to<JsonObject>();
    root["code"] = messierObject.code;
    root["NGC"] = messierObject.NGC;
    root["commonName"] = messierObject.commonName;
    root["objectType"] = messierObject.objectType;
    root["distance"] = messierObject.distance;
    root["constellation"] = messierObject.constellation;
    root["magnitude"] = messierObject.magnitude;
    JsonObject eqCoord = root.createNestedObject("eqCoord");
    eqCoord["ra"] = messierObject.eqCoord.ra; 
    eqCoord["dec"] = messierObject.eqCoord.dec; 
    String jsonString;
    serializeJson(doc, jsonString);
    request->send(200, "application/json", jsonString);
  });

  // init ws
  ws.onEvent(onEvent);
  server.addHandler(&ws);
  
  // Start server
  server.begin();
}

unsigned long webUpdateMillis = 0;
void loopWebserver(){
  // send update every 1000ms
  if(millis()-webUpdateMillis<1000) return;
  webUpdateMillis = millis();
  const size_t CAPACITY = JSON_OBJECT_SIZE(30);
  StaticJsonDocument<CAPACITY> doc;
  // create an object
  JsonObject root = doc.to<JsonObject>();
  root["date"] = String("")+day()+"/"+month()+"/"+year();
  root["time"] = String("")+hour()+":"+minute()+":"+second();
  JsonObject steppers = root.createNestedObject("steppers");
  steppers["azi"] = aziStepper.currentPosition();
  steppers["alt"] = altStepper.currentPosition();
  JsonObject target = root.createNestedObject("target");
  target["ra"] = targetEqCoord.ra;
  target["dec"] = targetEqCoord.dec;
  target["alt"] = targetHCoord.alt;
  target["azi"] = targetHCoord.azi;
  target["name"] = targetName;
  JsonObject pos = root.createNestedObject("position");
  pos["name"] = locationOnEarth.name;
  pos["lat"] = locationOnEarth.geoCoords.lat;
  pos["lon"] = locationOnEarth.geoCoords.lon;
  String jsonString;
  serializeJson(root, jsonString);
  web_print(jsonString);
}

void web_print(String value){
  // Serial.println(value);
  ws.textAll(value);
}
