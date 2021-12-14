/** 
 * storage index
 * 0-31    [32] : wifi SSID
 * 32-95   [64] : wifi Password
 * 96-165  [70] : TLE1
 * 166-235 [70] : TLE2
 * 236-239 [4] : Lat
 * 240-243 [4] : Lon
 * 244-259 [16] : last known IP.
 * 260-269 [10] : LOC
 * 270-271 [1]  : WIFI // is wifi set ?
 */
int EEPROM_SSID_OFFSET = 0;
int EEPROM_PWD_OFFSET  = 32;
int EEPROM_TLE1_OFFSET  = 96;
int EEPROM_TLE2_OFFSET = 166;
int EEPROM_LAT_OFFSET  = 236;
int EEPROM_LON_OFFSET  = 240;
int EEPROM_IP_OFFSET   = 244;
int EEPROM_LOC_OFFSET   = 260;
int EEPROM_WIFI_OFFSET  = 270;

void setupEEPROM(){
  EEPROM.begin(512);
}

/**
 * String
 */
int writeStringToEEPROM(int addrOffset, const String &strToWrite) {
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  EEPROM.commit();
  return addrOffset + len + 1;
}

String readStringFromEEPROM(int addrOffset) {
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  readCharArrayFromEEPROM(addrOffset+1, data, newStrLen);
  data[newStrLen] = '\0';
  return String(data);
}

/**
 * Float
 */
int writeFloatToEEPROM(int addrOffset, const float f){
  byte* b = (byte*) &f;
  EEPROM.write(addrOffset+0,b[0]);
  EEPROM.write(addrOffset+1,b[1]);
  EEPROM.write(addrOffset+2,b[2]);
  EEPROM.write(addrOffset+3,b[3]);
  EEPROM.commit();
  return addrOffset + 4;
}

float readFloatFromEEPROM(int addrOffset){
  byte b[4];
  b[0] = EEPROM.read(addrOffset);
  b[1] = EEPROM.read(addrOffset+1);
  b[2] = EEPROM.read(addrOffset+2);
  b[3] = EEPROM.read(addrOffset+3);
  float x = *((float*)(b));
  return x;
}

/**
 * Char Array
 */

int writeCharArrayToEEPROM(int addrOffset, char data[], int strLen){
  for (int i = 0; i < strLen; i++){
    EEPROM.write(addrOffset + i, data[i]);
  }
  EEPROM.commit();
  return addrOffset+strLen;
}

void readCharArrayFromEEPROM(int addrOffset, char data[], int strLen){
  for (int i = 0; i < strLen; i++){
    data[i] = EEPROM.read(addrOffset + i);
  }
}

/**
 * Boolean
 */
void writeBooleanToEEPROM(int addrOffset, boolean value){
  EEPROM.write(addrOffset, value?'Y':'N');
}

boolean readBooleanFromEEPROM(int addrOffset){
  return EEPROM.read(addrOffset) == 'Y';
}

/**
 * Unit Testing.
 */
#if TEST == 1

test(ReadWriteStringToEEPROM){
  String testData = "TEST QTRING";
  writeStringToEEPROM(0, testData);
  String result = readStringFromEEPROM(0);
  assertEqual(testData, result);
}

test(ReadWriteFloatToEEPROM){
  float testValue = 158.56;
  writeFloatToEEPROM(0, testValue);
  float result = readFloatFromEEPROM(0);
  assertEqual(testValue, result);
}

test(ReadWriteCharArrayToEEPROM){
  char* testValue = "hello";
  writeCharArrayToEEPROM(0, testValue, 6); // 6th char is end of string.
  char result[6];
  readCharArrayFromEEPROM(0, result, 6);
  assertEqual(testValue, result);
}

test(ReadWriteBooleanToEEPROM){
  writeBooleanToEEPROM(0, true);
  boolean result = readBooleanFromEEPROM(0);
  assertTrue(result);
  writeBooleanToEEPROM(0, false);
  result = readBooleanFromEEPROM(0);
  assertFalse(result);
}
#endif
