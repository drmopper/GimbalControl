/*
 * demoBoard2 Code for A-team Senior Design Presentation
 * Xi Chen/A-team
 * The purpose of the code is to control mode output with
 * this arduino board with respond to output from board1
 * Outputting Servo to set mode for gimbal
 * 
 */

#include <Servo.h> 
#include <SimpleTimer.h>

#define ModePin 10
#define BoardOne 12
#define DualMode 61
#define PitchMode 150
SimpleTimer timer;
Servo modeServo;
int ledOutput = HIGH;

void setup(){
  Serial.begin(9600);
  modeServo.attach(ModePin);
  pinMode(BoardOne,INPUT);
  modeServo.write(DualMode);
  pinMode(13, OUTPUT);//setting up led control
  digitalWrite(13,ledOutput);
}

void loop(){
    int boardOneInput = digitalRead(BoardOne);
    if(boardOneInput == HIGH){
      resetMode();
    }
    delay(200);
}

void resetMode(){
  modeServo.write(PitchMode);
  for(int a=0;a<25;a++){
    digitalWrite(13,LOW);
    delay(100);
    digitalWrite(13,HIGH);
    delay(100);
  }
  modeServo.write(DualMode);
}

