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

void up(int steps)   {altStepper.moveTo(altStepper.targetPosition()+steps);}
void down(int steps) {altStepper.moveTo(altStepper.targetPosition()-steps);}
void left(int steps) {aziStepper.moveTo(aziStepper.targetPosition()+steps);}
void right(int steps){aziStepper.moveTo(aziStepper.targetPosition()-steps);}

void setupStarpointer(){
  altStepper.setMaxSpeed(400);
  altStepper.setAcceleration(50);
  aziStepper.setMaxSpeed(400);
  aziStepper.setAcceleration(50);
  setLocationOnEarth(getLocation("MARS"));
}

void setLocationOnEarth(float lat, float lon){
  Location location = {"AUTO", "Browser Location", {lon, lat}};
  setLocationOnEarth(location);
}

void setLocationOnEarth(Location location){
  locationOnEarth = location;
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
