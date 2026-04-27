#include <SoftwareSerial.h>
#include "RichShieldPassiveBuzzer.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define right_speed 9 
#define right_fb 6
#define left_fb 5 
#define left_speed 3

SoftwareSerial espSerial(12, 13);     //initiallise serial communication with esp01
LiquidCrystal_I2C lcd(0x27,16,2);
String lastMessage = "";

void setup() {
  Serial.begin(9600);                                                               //debugging
  espSerial.begin(9600);                                                            //communication with esp01
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready for");
  lcd.setCursor(0,1);
  lcd.print("Manual"); 
}
void loop() {                                                                         //loop to check for client messages from esp01
  if (espSerial.available()) {
        String message = espSerial.readStringUntil('\n');
        message.trim(); // Remove whitespace
        Serial.println(message); // Debugging
        processRoute(message);
        
}
}

void processRoute(String message) {                                                   //define routes for processing
  if (message == "Client connected") {
    Serial.println("App_connected!");
    espSerial.println("App_connected!");  // Send confirmation response to ESP01
  }
  else if (message == "turnright") {
    move(1, 0, 100, 100);
    Serial.println("turned_right");
    espSerial.println("turned_right");
    delay(500);
    move(0,0,0,0);
  } 
  else if (message == "turnleft") {
    move(0, 1, 100, 100);
    Serial.println("turned_left");
    espSerial.println("turned_left");
    delay(500);
    move(0,0,0,0);
  } 
  else if (message == "forward") {
    move(1, 1, 100, 100);
    Serial.println("Going_Forward");
    espSerial.println("Going_Forward");
    delay(500);
    move(0,0,0,0);
  }
  else if (message == "reverse") {
    move(0, 0, 60, 60);
    Serial.println("Reversing_now");
    espSerial.println("Reversing_now");
    delay(500);
    move(0,0,0,0);
    }
  else {
    Serial.print(message);
    espSerial.print("message_recieved");
    LCD(message);
  }
}


void move(int RIGHT, int LEFT, int speedR, int speedL) {                     //Movement code, same as in autoRun.ino file
  if (RIGHT==LEFT&&!(RIGHT==speedR)){
    RIGHT=!RIGHT;
    LEFT=!LEFT;
  }
  if (RIGHT==1){
    speedR = 255-speedR;
  }
  if (LEFT==1){
    speedL =255-speedL;
  }
  digitalWrite(right_fb, RIGHT);
  analogWrite(right_speed, speedR);
  digitalWrite(left_fb, LEFT);
  analogWrite(left_speed, speedL);
}


void LCD(String message1) {                //allows remote debugging, shows the messages sent by client on LCD
    if (message1 != lastMessage) { //same as updateLCD in autoRun.ino file
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message1);
    lastMessage = message1;
    }
    }
