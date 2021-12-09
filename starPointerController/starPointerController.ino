/*********
  Patrice Delorme
  Controle le star pointer depuis le WIFI.
*********/

// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Ephemeris.h>
#include <TimeLib.h>
#include <AccelStepper.h>
#include "FS.h"
#include <Sgp4.h>
#include <EEPROM.h>


AccelStepper aziStepper(AccelStepper::HALF4WIRE, D0, D2, D1, D3);
AccelStepper altStepper(AccelStepper::HALF4WIRE, D5, D7, D6, D8);

enum TargetType {
  NoTarget = 0,
  ISSTarget = 1,
  SolarSystemTarget = 2,
  StellarTarget = 3
};

TargetType targetType = NoTarget;

// if -1 this is not a solar object.
SolarSystemObjectIndex solarSystemObjectIndex = Earth;
EquatorialCoordinates targetEqCoord;
HorizontalCoordinates targetHCoord;
String targetName = "Aucune";
unsigned long updateMillis = 0;

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  setupWifi();
  //setupSatTrack();
  setupWebserver();
  setupStarpointer();
}

void loop() {
  loopWebserver();
  loopStarpointer();
  // loopSatTrack();
}
