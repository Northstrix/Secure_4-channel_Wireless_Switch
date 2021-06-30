// Secure_4-channel_Wireless_Switch
// Firmware for the Transmitter board
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Secure_4-channel_Wireless_Switch

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DES.h>
#include <FS.h>
int debouncetime = 85; // debounce time in milliseconds
DES des;
// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0x40, 0xF5, 0x20, 0x33, 0x9A, 0xF5};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  byte ivone[8];
  byte ivtwo[8];
  byte ivthree[8];
  int action;
} struct_message;

// Create a struct_message called myData
struct_message myData;

void encryptfirstiv(String plaintext)
{
  byte in[] = {32, 32, 32, 32, 32, 32, 32, 32};
  for (int i = 0; i < 8; i++)
  {
      char x = plaintext[i];
      in[i] = int(x);
  }
  byte out[8];
  byte key[] = { 
                  0x1C, 0x11, 0x52, 0xB8, 0x9A, 0x61, 0xEB, 0x60,
                  0x9E, 0x55, 0x05, 0x9D, 0xE5, 0x7E, 0x38, 0xE9,
                  0x8A, 0x2B, 0xF7, 0x6A, 0xD9, 0x8C, 0x5C, 0x56,
                };

 
  //encrypt
  des.tripleEncrypt(out, in, key);
  for (int i=0; i<8; i++){
  myData.ivone[i] = out[i];
  }  
 }

void encryptsecondiv(String plaintext)
{
  byte in[] = {32, 32, 32, 32, 32, 32, 32, 32};
  for (int i = 0; i < 8; i++)
  {
      char x = plaintext[i];
      in[i] = int(x);
  }
  byte out[8];
  byte key[] = { 
                  0x58, 0xF4, 0x45, 0x1C, 0x53, 0x36, 0x4C, 0x76,
                  0xF8, 0xC4, 0xD0, 0x79, 0x2F, 0xAF, 0x54, 0x33,
                  0x9C, 0xDC, 0x54, 0x45, 0x31, 0x50, 0xAE, 0xA2,
                };
 
  //encrypt
  des.tripleEncrypt(out, in, key);
  for (int i=0; i<8; i++){
  myData.ivtwo[i] = out[i];
  }  
 }

void encryptthirdiv(String plaintext)
{
  byte in[] = {32, 32, 32, 32, 32, 32, 32, 32};
  for (int i = 0; i < 8; i++)
  {
      char x = plaintext[i];
      in[i] = int(x);
  }
  byte out[8];
  byte key[] = { 
                  0x14, 0x02, 0xE5, 0x8D, 0x81, 0x09, 0x98, 0x59,
                  0x92, 0x74, 0x02, 0x92, 0x40, 0x5E, 0x01, 0xD9,
                  0xFF, 0x3E, 0x9C, 0xE1, 0x25, 0xD7, 0x4A, 0x5B,
                };
 
  //encrypt
  des.tripleEncrypt(out, in, key);
  for (int i=0; i<8; i++){
  myData.ivthree[i] = out[i];
  }  
 }

 
// callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0){
    Serial.println("Delivery success");
  }
  else{
    Serial.println("Delivery fail");
  }
}
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

void stuff_sender(int act){
  // Read IVs
  String f = readFile(SPIFFS, "/FIV.txt");
  String s = readFile(SPIFFS, "/SIV.txt");
  String t = readFile(SPIFFS, "/TIV.txt");
  // Convert IVs to the int
  unsigned int fir = f.toInt();
  unsigned int sec = s.toInt();
  unsigned int thi = t.toInt();
  // Increment IVs
  fir++;
  sec++;
  thi++;
  // Convert IVs back to the strings
  f = String(fir);
  s = String(sec);
  t = String(thi);
  // Save new IVs to the SPIFFS
  writeFile(SPIFFS, "/FIV.txt", f);
  writeFile(SPIFFS, "/SIV.txt", s);
  writeFile(SPIFFS, "/TIV.txt", t);
  // Encrypt stuff
  encryptfirstiv(f);
  encryptsecondiv(s);
  encryptthirdiv(t);
  myData.action = act;
  // Send message via ESP-NOW
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
}
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  // Initialize input pins
  pinMode(14, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  pinMode(15, INPUT);
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
  
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}
 
void loop() {
  
  if(analogRead(14)<100){
    delay(debouncetime);
      if(analogRead(14)<100){
    stuff_sender(1);
  }
  }

  if(analogRead(12)<100){
    delay(debouncetime);
      if(analogRead(12)<100){
    stuff_sender(2);
  }
  }

  if(analogRead(13)<100){
    delay(debouncetime);
      if(analogRead(13)<100){
    stuff_sender(3);
  }
  }

  if(analogRead(2)<100){
    delay(debouncetime);
      if(analogRead(2)<100){
    stuff_sender(4);
  }
  }
}
