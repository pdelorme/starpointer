const int RoundSteps = 2048;
const int azi360Steps = -RoundSteps * 6;
const int alt360Steps = RoundSteps * 4;

// laser is pointing to polaris.
// set pointer steps to hypothetical 0.
void setLaserPositionFromPolaris(){
  HorizontalCoordinates polarisHCoord = Ephemeris::equatorialToHorizontalCoordinatesAtDateAndTime(polarisEqCoord, day(), month(), year(), hour(), minute(), second());
  aziStepper.setCurrentPosition(polarisHCoord.azi/360*azi360Steps); // steps from azimuth.
  altStepper.setCurrentPosition(polarisHCoord.alt/360*alt360Steps); // Steps from altitude.
}

void goToEqCoords(EquatorialCoordinates eqCoordinates){
  targetEqCoord = eqCoordinates;
  HorizontalCoordinates targetHCoord = Ephemeris::equatorialToHorizontalCoordinatesAtDateAndTime(eqCoordinates, day(), month(), year(), hour(), minute(), second());
  goToHCoords(targetHCoord.alt,targetHCoord.azi); // Steps from altitude.
}

void goToHCoords(float alt, float azi){
  // min altitude is 0.
  if(alt<0) alt = 0;
  if(alt>180) alt = 180;
  altStepper.moveTo(alt/360*alt360Steps); // Steps from altitude.  
  aziStepper.moveTo(azi/360*azi360Steps); // steps from azimuth.
  targetHCoord = {alt, azi};
}

// goto 0,0
void goHome(){
  goToHCoords(0,0);
}

void laserOn(boolean isOn){
  digitalWrite (LASER_PIN, isOn?HIGH:LOW);
}

void up(int steps)   {altStepper.moveTo(altStepper.targetPosition()+steps);}
void down(int steps) {altStepper.moveTo(altStepper.targetPosition()-steps);}
void left(int steps) {aziStepper.moveTo(aziStepper.targetPosition()+steps);}
void right(int steps){aziStepper.moveTo(aziStepper.targetPosition()-steps);}

void setupStarpointer(){
  // steppers
  altStepper.setMaxSpeed(400);
  altStepper.setAcceleration(50);
  aziStepper.setMaxSpeed(400);
  aziStepper.setAcceleration(50);
  // laser
  pinMode(LASER_PIN, OUTPUT);
  // location
  String location = readStringFromEEPROM(EEPROM_LOC_OFFSET);
  if(location=="MANUAL"){
    float lat = readFloatFromEEPROM(EEPROM_LAT_OFFSET);
    float lon = readFloatFromEEPROM(EEPROM_LON_OFFSET);
    setPositionOnEarth(lat, lon, false);
  } else {
    setLocationOnEarth(getLocation(location), false); 
  }
}

void setPositionOnEarth(float lat, float lon, boolean save){
  Location location = {"MANUAL", "Manuel", {lon, lat}};
  setLocationOnEarth(location, save);
}

Location getLocationOnEarth(){
  String locCode = readStringFromEEPROM(EEPROM_LOC_OFFSET);
  if(locCode=="MANUAL"){
    float lat = readFloatFromEEPROM(EEPROM_LAT_OFFSET);
    float lon = readFloatFromEEPROM(EEPROM_LON_OFFSET);
    Location location = {"MANUAL", "Manuel", {lon, lat}};
    return location;
  } else {
    return getLocation(locCode);
  }
}
void setLocationOnEarth(Location location, boolean save){
  debugln("setLocationOnEarth:"+location.name);
  locationOnEarth = location;
  if(save){
    writeStringToEEPROM(EEPROM_LOC_OFFSET, location.code);
    writeFloatToEEPROM(EEPROM_LAT_OFFSET, location.geoCoords.lat);
    writeFloatToEEPROM(EEPROM_LON_OFFSET, location.geoCoords.lon);
  }
  Ephemeris::setLocationOnEarth(location.geoCoords.lat, location.geoCoords.lon);
}

unsigned long solarSystemUpdateMillis = 0;
void loopStarpointer(){
  if(targetType == SolarSystemTarget && (millis()-solarSystemUpdateMillis>1000)){
    solarSystemUpdateMillis = millis();
    SolarSystemObject solarSystemObject = Ephemeris::solarSystemObjectAtDateAndTime(solarSystemObjectIndex, day(), month(), year(), hour(), minute(), second());
    goToEqCoords(solarSystemObject.equaCoordinates);
  }
  altStepper.run();
  aziStepper.run();
}

/**
 * Unit Testing.
 */
#if TEST == 1

test(setLocationOnEarthFalse){
  Location loc = getLocation("LYON");
  setLocationOnEarth(loc, false);
  assertEqual(loc.name, locationOnEarth.name);
  assertEqual(loc.code, locationOnEarth.code);
  assertEqual(loc.geoCoords.lat, locationOnEarth.geoCoords.lat);
  assertEqual(loc.geoCoords.lon, locationOnEarth.geoCoords.lon);
}

test(setLocationOnEarthTrue){
  Location loc = getLocation("BREST");
  setLocationOnEarth(loc, true);
  Location loc2 = getLocationOnEarth();
  assertEqual(loc.name, loc2.name);
  assertEqual(loc.code, loc2.code);
  assertEqual(loc.geoCoords.lat, loc2.geoCoords.lat);
  assertEqual(loc.geoCoords.lon, loc2.geoCoords.lon);
}
#endif
