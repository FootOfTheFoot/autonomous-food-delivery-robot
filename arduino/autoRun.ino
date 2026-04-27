#include "RichShieldPassiveBuzzer.h"
#include <PinChangeInterrupt.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define PassiveBuzzerPin 4
#define Hit_aPin 2
#define IR1Pin A0 
#define IR2Pin A1
#define trigpin1 11
#define echopin1 10
#define right_speed 9 
#define right_fb 6
#define left_speed 5
#define left_fb 3
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C4 262
#define NOTE_C5 523
#define NOTE_D4 294
#define NOTE_D5  587
#define NOTE_E4 330 
#define NOTE_E5  659
#define NOTE_G4 392
#define BEEP_FREQUENCY 500
#define BEEP_FREQUENCY2 1000
#define NO_SOUND 0
LiquidCrystal_I2C lcd(0x27,16,2);
PassiveBuzzer buz(PassiveBuzzerPin);

const static int blockingsound[] = {BEEP_FREQUENCY2, NO_SOUND}; //buzzer sounds for respective movements
const static int blockingnotedur[] = {4, 8};
const static int reversingsound[] = {BEEP_FREQUENCY, NO_SOUND};
const static int reversingnotedur[] = {4, 2};
const static int stoppingsound[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
const static int stoppingnotedur[] = {4, 4, 4, 2};
const static int forwardsound[] = {NOTE_C4, NOTE_G4, NOTE_D4, NOTE_A4};
const static int forwardnotedur[] = {4, 8, 4, 8};
const static int turningsound[] = {NOTE_D5, NOTE_E5, NOTE_A4};
const static int turningnotedur[] = {8, 8, 8};

int sizeblocking = sizeof(blockingsound) / sizeof(blockingsound[0]); //so that the "for" loop in playNotes know how many notes to play
int sizereversing = sizeof(reversingsound) / sizeof(reversingsound[0]);
int sizeturning = sizeof(turningsound) / sizeof(turningsound[0]);
int sizeforward = sizeof(forwardsound) / sizeof(forwardsound[0]);
int sizestop = sizeof(stoppingsound) / sizeof(stoppingsound[0]);
int RIGHT_IR;
int LEFT_IR;
int movementCounter = 0; 
int lastDirection = 1; 
int resetCount = 0;
int counterA = 0;
int counterB = 0;
int counterC = 0;
String lastMessage = "";

void setup() {
  Serial.begin(9600);
  pinMode(IR1Pin, INPUT);
  pinMode(IR2Pin, INPUT);
  pinMode(right_fb, OUTPUT);
  pinMode(right_speed, OUTPUT);
  pinMode(left_fb, OUTPUT);
  pinMode(left_speed, OUTPUT);
  pinMode(trigpin1, OUTPUT);
  pinMode(echopin1, INPUT);
  pinMode(Hit_aPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Hit_aPin), handleInterrupt, FALLING); //if target board changes from 1 to 0, run handleInterrupt
  attachPCINT(digitalPinToPCINT(IR1Pin), drivemotor, CHANGE); //if any of the IR pin changes, run drivemotor, this is because buzzer sounds has delay, hence we need this to change direction instantly
  attachPCINT(digitalPinToPCINT(IR2Pin), drivemotor, CHANGE);
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Ready to"); //display this when we turn on the robot at the start
  lcd.setCursor(0,1);
  lcd.print("Send Food"); 
}

void loop() {
  int dist = getdist();
  drivemotor(dist);
  if (counterA>0) //so it does not play sound until we hit the target board
  {
    Movementsounds(dist);
    LCDstatement(dist);
  }
}

int getdist() {
  digitalWrite(trigpin1, LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin1, LOW);
  return pulseIn(echopin1, HIGH) / 58;
}

void drivemotor(int dist) {
  RIGHT_IR = digitalRead(IR1Pin); 
  LEFT_IR = digitalRead(IR2Pin);
  if (counterA==0){ //Ensures the motor doesn't move before the target board is hit, as interrupt pins might run drivemotor
    move(0,0,0,0);
  }
  else if (!(counterA%2)){ //Allows the robot to turn anticlockwise when the target board is hit the 2nd time, 4th time, etc
    if (counterC==0){
      move (1,1,100,100); //the robot moves forward slightly so that it does not detect black for the next code
      delay(200);
      move (0,0,0,0); //stops
      LCDstatement(dist); //displays either sending food or returning home for 5 seconds so that the user knows whats gonna happen.
      delay(5000);
      counterC++;
      counterB=1;
    }
    if (counterB==1){
      move(0,1,100,100); //turn anticlockwise 
      delay(100);
    
    if (LEFT_IR&&RIGHT_IR) //stops when both IR sensors detect black, and allows the drivemotor to follow the line again with counterA++
    {
      move(0,0,0,0);
      movementCounter=0;
      counterB=0;
      counterA++; //important so that counterA is no longer a multiple of 2.
      counterC=0;
    }
    }
  }
  else if (movementCounter >= 16||dist<15) { //stops when there is an object infront or when it moves front and back 16 times
      move(0,0,0,0);
    }
  else if (RIGHT_IR == 0 && LEFT_IR == 0) { //reverse when both ir detect white, as some turns are too sharp
      move(RIGHT_IR, LEFT_IR, 60, 60);
      if (lastDirection != 0) { //movementCounter is to allow the robot to move back and forth at the end of the line a few times and stop
        movementCounter++;
        lastDirection = 0;
        resetCount=0;
      }
    }
  else if (RIGHT_IR == 1 && LEFT_IR == 1) { //forward when both ir detect black
      move(RIGHT_IR, LEFT_IR, 100, 100);
      if (lastDirection != 1) {
        movementCounter++;
        lastDirection = 1;
        resetCount=0;
      } 
      else {
        resetCount++; //if it goes forward for 8 conesecutive counts, reset the movementCounter. 
      }
      if (resetCount == 8) {
          movementCounter = 0;
          resetCount = 0;
      }
    }
  else if (RIGHT_IR == 1 && LEFT_IR == 0) { //turn right
      move(RIGHT_IR, LEFT_IR, 60, 120);
    }
  else if (RIGHT_IR == 0 && LEFT_IR == 1) { //turn left
      move(RIGHT_IR, LEFT_IR, 120, 60);
    }
}

void Movementsounds(int dist) {
  if (movementCounter>=16){
    playNotes(stoppingsound, stoppingnotedur, sizestop);
  }
  else if (dist<15){ //sound if theres an object within 15cm infront of the robot
     playNotes(blockingsound, blockingnotedur, sizeblocking);
    }
  else if (digitalRead(right_fb) == 1 && digitalRead(left_fb) == 1) {
     playNotes(reversingsound, reversingnotedur, sizereversing);
    }
  else if (digitalRead(right_fb) != digitalRead(left_fb)) {
     playNotes(turningsound, turningnotedur, sizeturning);
    }
  else if (digitalRead(right_fb) == 0 && digitalRead(left_fb) == 0) {
     playNotes(forwardsound, forwardnotedur, sizeforward);
    }
}

void LCDstatement(int dist) {
    if (!(counterA % 4)) {
        updateLCD("Sending", "Food");
    } else if (!(counterA % 2)) {
        updateLCD("Returning", "Home");
    } else if (movementCounter >= 16 && !((counterA - 3) % 4)) { //if counterA is 3, 7, 11, it would display Back At Home.
        updateLCD("Back At", "Home");
    } else if (movementCounter >= 16) {
        updateLCD("Destination", "reached");
    } else if (dist < 15) {
        updateLCD("Please move", "aside");
    } else if (digitalRead(right_fb) == 1 && digitalRead(left_fb) == 1) {
        updateLCD("Reversing", "");
    } else if (digitalRead(right_fb) != digitalRead(left_fb)) {
        updateLCD("Turning", "");
    } else if (digitalRead(right_fb)==0&&digitalRead(left_fb)==0) {
        updateLCD("Moving Forward", "");
    }
}

void handleInterrupt() //if targetBoard goes from 1 to 0, counterA++
{
  counterA ++;
}

void move(int RIGHT, int LEFT, int speedR, int speedL) {
  if (RIGHT==LEFT&&!(RIGHT==speedR)){ //the point of RIGHT==speedR is to ensure that if all the values are 0, it would stop as intended. 
    RIGHT=!RIGHT;
    LEFT=!LEFT;
  }
  if (RIGHT==1){ //e.g. for the robot to move backwards at 100 speed, the right_speed has to be 155, if right_fb is 1
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

void playNotes(const int *notes, const int *durations, int size) {
  
    for (int i = 0; i < size; i++) { 
        int noteDur = 800 / durations[i];
        buz.playTone(notes[i], noteDur);
    }
}

void updateLCD(String message, String message1) {
    if (message != lastMessage) { //e.g. if message is "Sending" and lastMessage is "Sending", the LCD would not clear, hence prevents flickering by only updating the display when necessary
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(message);
        lcd.setCursor(0,1);
        lcd.print(message1);
        lastMessage = message;
    }
}
