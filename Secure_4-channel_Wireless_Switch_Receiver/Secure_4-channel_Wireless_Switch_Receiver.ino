// Secure_4-channel_Wireless_Switch
// Firmware for the Receiver board
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Secure_4-channel_Wireless_Switch

#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <espnow.h>
#include <DES.h>
#include <FS.h>
#include "GBUS.h"
SoftwareSerial mySerial(0, 2); // RX, TX
GBUS bus(&mySerial, 5, 20);
DES des;
String firstdec;
String seconddec;
String thirddec;
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  byte ivone[8];
  byte ivtwo[8];
  byte ivthree[8];
  int action;
} struct_message;

struct myStruct {
  int p;
};

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

// Create a struct_message called myData
struct_message myData;
  void decryptfirst(byte ct[]){
  //decrypt
  byte out[8];
  byte key[] = { 
                  0x1C, 0x11, 0x52, 0xB8, 0x9A, 0x61, 0xEB, 0x60,
                  0x9E, 0x55, 0x05, 0x9D, 0xE5, 0x7E, 0x38, 0xE9,
                  0x8A, 0x2B, 0xF7, 0x6A, 0xD9, 0x8C, 0x5C, 0x56,
                };
  firstdec = "";           
  des.tripleDecrypt(out, ct, key);
    for (int i = 0; i < 8; i++){
    char x = out[i];
    firstdec += x;
    }
}

  void decryptsecond(byte ct[]){
  //decrypt
  byte out[8];
  byte key[] = { 
                  0x58, 0xF4, 0x45, 0x1C, 0x53, 0x36, 0x4C, 0x76,
                  0xF8, 0xC4, 0xD0, 0x79, 0x2F, 0xAF, 0x54, 0x33,
                  0x9C, 0xDC, 0x54, 0x45, 0x31, 0x50, 0xAE, 0xA2,
                };
  seconddec = "";            
  des.tripleDecrypt(out, ct, key);
    for (int i = 0; i < 8; i++){
    char x = out[i];
    seconddec += x;
    }
}

  void decryptthird(byte ct[]){
  //decrypt
  byte out[8];
  byte key[] = { 
                  0x14, 0x02, 0xE5, 0x8D, 0x81, 0x09, 0x98, 0x59,
                  0x92, 0x74, 0x02, 0x92, 0x40, 0x5E, 0x01, 0xD9,
                  0xFF, 0x3E, 0x9C, 0xE1, 0x25, 0xD7, 0x4A, 0x5B,
                };
  thirddec = "";            
  des.tripleDecrypt(out, ct, key);
    for (int i = 0; i < 8; i++){
    char x = out[i];
    thirddec += x;
    }
}

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  byte ivone[8];
  byte ivtwo[8];
  byte ivthree[8];
  // Extract IVs from the structure
  for (int i = 0; i<8; i++){
  ivone[i] = myData.ivone[i];
  }
  for (int i = 0; i<8; i++){
  ivtwo[i] = myData.ivtwo[i];
  }
  for (int i = 0; i<8; i++){
  ivthree[i] = myData.ivthree[i];
  }
  // Decrypt IVs
  decryptfirst(ivone);
  decryptsecond(ivtwo);
  decryptthird(ivthree);
  // Extract IVs from SPIFFS
  String f = readFile(SPIFFS, "/FIV.txt");
  String s = readFile(SPIFFS, "/SIV.txt");
  String t = readFile(SPIFFS, "/TIV.txt");
  // Convert stored IVs to the ints
  unsigned int firststored = f.toInt();
  unsigned int secondstored = s.toInt();
  unsigned int thirdstored = t.toInt();
  // Add 50 to stored IVS
  unsigned int firstplus = firststored + 50;
  unsigned int secondplus = secondstored + 50;
  unsigned int thirdplus = thirdstored + 50;
  // Convert received IVs to the ints
  unsigned int fir = firstdec.toInt();
  unsigned int sec = seconddec.toInt();
  unsigned int thi = thirddec.toInt();
  // Print the IVs in the Serial Monitor
  Serial.println("Stored");
  Serial.println(f);
  Serial.println(s);
  Serial.println(t);
  Serial.println("Received");
  Serial.println(fir);
  Serial.println(sec);
  Serial.println(thi);
  if( fir > firststored && sec > secondstored && thi > thirdstored && fir < firstplus && sec < secondplus && thi < thirdplus){
  writeFile(SPIFFS, "/FIV.txt", firstdec);
  writeFile(SPIFFS, "/SIV.txt", seconddec);
  writeFile(SPIFFS, "/TIV.txt", thirddec);
  myStruct data;
  data.p = myData.action;
  bus.sendData(3, data);
  }
  else{
  myStruct data;
  data.p = 5;
  bus.sendData(3, data);
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  // Initialise software serial
  mySerial.begin(9600);
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
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
}
