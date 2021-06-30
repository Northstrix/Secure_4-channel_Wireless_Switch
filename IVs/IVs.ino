// Secure_4-channel_Wireless_Switch
// Initialization Vector Setter
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Secure_4-channel_Wireless_Switch

#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
  #include <SPIFFS.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
  #include <Hash.h>
  #include <FS.h>
#endif
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- empty file or failed to open file");
    return String();
  }
  String fileContent;
  while(file.available()){
    fileContent+=String((char)file.read());
  }
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, String IV){
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(IV)){
    Serial.println("- file written");
  } else {
    Serial.println("- write failed");
  }
}
void setup() {
  Serial.begin(115200);
  // Initialize SPIFFS
  #ifdef ESP32
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #else
    if(!SPIFFS.begin()){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
  #endif
  
  String fiv = "48391741";
  String siv = "51206333";
  String tiv = "60692408";
  
  writeFile(SPIFFS, "/FIV.txt", fiv);
  writeFile(SPIFFS, "/SIV.txt", siv);
  writeFile(SPIFFS, "/TIV.txt", tiv);

}

void loop() {
  String f = readFile(SPIFFS, "/FIV.txt");
  String s = readFile(SPIFFS, "/SIV.txt");
  String t = readFile(SPIFFS, "/TIV.txt");
  unsigned int fir = f.toInt();
  unsigned int sec = s.toInt();
  unsigned int thi = t.toInt();
  Serial.println("IVs:");
  Serial.println(fir);
  Serial.println(sec);
  Serial.println(thi);
  delay(5000);

}
