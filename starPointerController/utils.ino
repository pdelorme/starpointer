void debug(String message){
  #if DEBUG == 1
  Serial.print(message);
  #endif
}

void debugln(String message){
  #if DEBUG == 1
  Serial.println(message);
  #endif
}

/**
 * splits source at sep into left and right
 */
void split(String source, char sep, String &left, String &right){
  int index = source.indexOf(sep);
  left  = source.substring(0,index);
  right = source.substring(index+1);
}

/**
 * search for key argument in the line and returns its value.
 * @param line : the argument string in the form "KEY1=value,KEY2=value"
 * @param key : the wanted argument
 * @return the argument value of null if not found.
 */
String cmdArgValue(String line, String key){
  if(line == NULL){
    return line;
  }
  int index = line.indexOf(",");
  String argument = line;
  String endOfLine;
  if(index>0){
    argument = line.substring(0,index);
    endOfLine = line.substring(index+1);
  }
  if(argument.startsWith(key)){
    index = argument.indexOf("=");
    return argument.substring(index+1);
  }
  return cmdArgValue(endOfLine, key);
}

/**
 * Unit Testing.
 */
#if TEST == 1

test(split){
  String testData = "TEST:STRING";
  String left, right;
  split(testData, ':', left, right);
  assertEqual("TEST", left);
  assertEqual("STRING", right);
}

test(cmdArgValue){
  String testData = "ARG1=ONE,ARG2=TWO,ARG3=THREE";
  assertEqual("ONE", cmdArgValue(testData, "ARG1"));
  assertEqual("TWO", cmdArgValue(testData, "ARG2"));
  assertEqual("THREE", cmdArgValue(testData, "ARG3"));
}
#endif
