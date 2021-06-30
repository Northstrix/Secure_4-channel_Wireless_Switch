// Secure_4-channel_Wireless_Switch
// Firmware for the Arduino
// Distributed under the MIT License
// © Copyright Maxim Bortnikov 2021
// For more information please visit
// https://github.com/Northstrix/Secure_4-channel_Wireless_Switch

#include <SoftwareSerial.h>
SoftwareSerial mySerial(9, 10); // RX, TX
#include "GBUS.h"
#include <TM1637.h>
int CLK = 11;
int DIO = 12;
TM1637 tm(CLK,DIO);
GBUS bus(&mySerial, 3, 20);
// Variables to control the channels
bool ch1;
bool ch2;
bool ch3;
bool ch4;
String str;
int c = 0;
int a = 0;

struct myStruct {
  int p;
};

void setup() {
  // Initialize output pins
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  ch1 = false;
  ch2 = false;
  ch3 = false;
  ch4 = false;
  mySerial.begin(9600);
  // Initialize the display
  tm.init();
  //set brightness; 0-7
  tm.set(5);
  // Display zero
  displayNumber(a);

}

void displayNumber(int num){   
    tm.display(3, num % 10);   
    tm.display(2, num / 10 % 10);   
    tm.display(1, num / 100 % 10);   
    tm.display(0, num / 1000 % 10);
}

void loop() {
  // Switch the channel state
  if (c == 1 && ch1 == false)
  ch1 = true;
  else if (c == 1 && ch1 == true)
  ch1 = false;
  else if (c == 2 && ch2 == false)
  ch2 = true;
  else if (c == 2 && ch2 == true)
  ch2 = false;
  else if (c == 3 && ch3 == false)
  ch3 = true;
  else if (c == 3 && ch3 == true)
  ch3 = false;
  else if (c == 4 && ch4 == false)
  ch4 = true;
  else if (c == 4 && ch4 == true)
  ch4 = false;
  else if (c == 5){
  a++;
  displayNumber(a);
  }
  // Translate the value of bool into the voltage level
  if(ch1 == true)
  pinMode(2,HIGH);
  else
  pinMode(2,LOW);
  if(ch2 == true)
  pinMode(3,HIGH);
  else
  pinMode(3,LOW);
  if(ch3 == true)
  pinMode(4,HIGH);
  else
  pinMode(4,LOW);
  if(ch4 == true)
  pinMode(5,HIGH);
  else
  pinMode(5,LOW);
  bus.tick();
  c = 0;
  if (bus.gotData()) {
    myStruct data;
    bus.readData(data);
    c = data.p;
  }
}
