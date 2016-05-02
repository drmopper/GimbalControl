/*
 * demoBoard2 Code for A-team Senior Design Presentation
 * Xi Chen/A-team
 * 
 * The purpose of the code is to take command from pc interface
 * and output corresponding signals to turn the gimbal to desired 
 * position
 * 
 */
#include <avr/pgmspace.h>
#include <Servo.h> 
#include <SimpleTimer.h>

#define YawPin 9
#define PitchPin 10
#define BoardTwo 12

#define YawHalt 90
#define YawCCW 48
#define YawCW 138

SimpleTimer timer;
Servo pitchServo;
Servo yawServo;

const int pitchMap[] PROGMEM= {46,47,47,48,48,49,49,50,50,51,51,52,52,53,54,54,55,55,56,56,57,57,58,58,59,60,60,61,61,62,62,63,63,64,64,65,65,66,66,67,68,68,69,69,70,70,71,71,72,72,73,74,74,75,75,76,76,77,77,78,78,79,80,80,81,81,82,82,83,83,84,84,85,85,86,87,87,88,88,89,89,90,90,91,91,92,93,93,94,94,95,95,96,96,97,97,98,98,99,100,100,101,101,102,102,103,103,104,104,105,105,106,107,107,108,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,117,117,118,118,119};
int currentPitch;
int currentYaw;

void setup() {
  Serial.begin(9600);
  pitchServo.attach(PitchPin);
  yawServo.attach(YawPin);
  currentPitch = 0;
  currentYaw = 0;
  pitchServo.write(pitchMap[currentPitch+90]);//set pitch position to 90
  yawServo.write(YawHalt);//halt any yaw movement
}

void loop() {
  // put your main code here, to run repeatedly:

}

void readInput(){
  
}

void resetMode(){
  Serial.println(F("Resetting, Please wait for reset to complete..."));
  digitalWrite(BoardTwo,HIGH);
  delay(5000);
  digitalWrite(BoardTwo,LOW);
  Serial.println(F("Mode Reset Complete"));
}

