/**
 * Locations on earth
 */
struct Location {
  String code;
  String name;
  GeocentricCoordinates geoCoords;
  float alt;
};

const int locationsLength = 1;
Location locations[] = {
  { "MARS", "Marseille",   {5.369780,  43.296482},  0 }, // Marseille coordinates from latlon.net : LatLon = 43.296482, 5.369780 ; GPS =  43° 17' 47.3352'' N, 5° 22' 11.208'' E
  { "LYON", "Lyon",        {4.8320114, 45.7578137}, 0 }, 
  { "GRE", "Grenoble",     {5.7357819, 45.1875602}, 0 },
  { "PARIS", "Paris",      {2.320041,  48.8588897}, 0 },
  { "BORD", "Bordeaux",    {-0.5800364,44.841225},  0 },
  { "BREST", "Brest",      {-4.4860088,48.3905283}, 0 },
  { "STRAS", "Strasbourg", {7.7507127, 48.584614},  0 }
};

struct Location getLocation(String code){
  for(int i=0;i<locationsLength;i++){
    if(locations[i].code == code){
      return locations[i];
    }
  }  
}

Location locationOnEarth;

/** 
 * Polaris coordinates from wikipedia  02h 31m 48.7s, +89° 15′ 51″ 
 */
EquatorialCoordinates polarisEqCoord = {
  Ephemeris::hoursMinutesSecondsToFloatingHours(2,31,48.7),
  Ephemeris::degreesMinutesSecondsToFloatingDegrees(89,15,51) 
};

/**
 * Messier Objects
 */
struct MessierObject {
    String code;
    String NGC;
    String commonName;
    String objectType;
    String distance;
    String constellation;
    float magnitude;
    EquatorialCoordinates eqCoord;
};
const int messierDataLength = 7;
MessierObject messierData[messierDataLength] = {
  { "M45", "–", "Pleiades", "Open cluster", "0.39–0.46", "Taurus", 1.6, {3.7900000021, 24.1166666669} },
  { "M24", "IC 4715", "Small Sagittarius Star Cloud", "Milky Way star cloud", "~10", "Sagittarius", 2.5, {18.2833333339, -18.5500000011} },
  { "M7", "NGC 6475", "Ptolemy Cluster", "Open cluster", "0.65–1.31", "Scorpius", 3.3, {17.89755555846, -34.7927777801} },
  { "M31", "NGC 224", "Andromeda Galaxy", "Spiral galaxy", "2,430–2,650", "Andromeda", 3.4, {0.71230555794, 41.2691666674} },
  { "M44", "NGC 2632", "Beehive Cluster or Praesepe", "Open cluster", "0.577", "Cancer", 3.7, {8.67333333468, 19.9833333353} },
  { "M42", "NGC 1976", "Orion Nebula", "H II region nebula", "1.324–1.364", "Orion", 4, {5.5833333345, -5.3911111125} },
  { "M6", "NGC 6405", "Butterfly Cluster", "Open cluster", "1.6", "Scorpius", 4.2, {17.66833333467, -32.2166666671} }
};

struct MessierObject getMessierObject(String code){
  for(int i=0;i<messierDataLength;i++){
    if(messierData[i].code == code){
      return messierData[i];
    }
  }
}

/**
 * Solar System Objects
 */
struct SolarSystemRef {
  String code;
  String name;
  SolarSystemObjectIndex index;
};
const int solarSystemDataLength = 9;
SolarSystemRef solarSystemData[] = {
 { "S1", "Moon",    EarthsMoon },
 { "S2", "Sun",     Sun },
 { "S3", "Mercury", Mercury },
 { "S4", "Venus",   Venus },
 { "S5", "Mars",    Mars },
 { "S6", "Jupiter", Jupiter },
 { "S7", "Saturn",  Saturn },
 { "S8", "Uranus",  Uranus },
 { "S9", "Neptune", Neptune }
};

struct SolarSystemRef getSolarObject(String code){
  for(int i=0;i<solarSystemDataLength;i++){
    if(solarSystemData[i].code == code){
      return solarSystemData[i];
    }
  }
}
