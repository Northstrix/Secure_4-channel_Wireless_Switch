// Secure_4-channel_Wireless_Switch
// Firmware for the Trash Sender
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Secure_4-channel_Wireless_Switch

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DES.h>
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
                  0xC9, 0x69, 0x04, 0xD9, 0x2E, 0xA2, 0x56, 0x96,
                  0x42, 0xCF, 0x17, 0x6C, 0x31, 0xD1, 0x31, 0xE9,
                  0x8D, 0x91, 0xB7, 0x39, 0x62, 0x44, 0xC7, 0x21,
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
                  0x57, 0xDA, 0x21, 0x4A, 0x5A, 0xE5, 0xCD, 0xFA,
                  0x53, 0xD8, 0xE7, 0x2B, 0x5F, 0xEB, 0x76, 0x34,
                  0x6E, 0xB4, 0xC5, 0x18, 0x6F, 0xA3, 0xE2, 0x7F,
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
                  0x7A, 0x87, 0x6F, 0x26, 0x1D, 0xA6, 0x3E, 0x67,
                  0xDB, 0xCF, 0x4E, 0x9A, 0x31, 0xF5, 0x6A, 0xEE,
                  0xB8, 0x91, 0x7F, 0x3C, 0x27, 0xF4, 0xBC, 0xEF,
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

void stuff_sender(int act){
  unsigned int fir = random(1000000000);
  unsigned int sec = random(1000000000);
  unsigned int thi = random(1000000000);
  // Convert IVs to the strings
  String f = String(fir);
  String s = String(sec);
  String t = String(thi);
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
  WiFi.mode(WIFI_STA);
  randomSeed(analogRead(A0));
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
 stuff_sender(1);
 delay(100);
}
