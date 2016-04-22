/*
 * PWM TEST FOR GIMBAL CONTROL
 * Xi Chen/A-Team
 * Currrently Controlling Mode Pin and Roll Pin with 500hz pwms
 * Comment/Uncomment in define to change Pitch Control Mode
 * 
 * PitchMode 0 for PWMOUTPUT for 500hz pwm output
 * OUTPUT Between 0-255 
 * PitchMode 1 for SERVOOUTPUT for Servo output
 * OUTPUT Between 0-359
 * Please Check Arduino Documentation before changing assigned pin
 * to ensure correct output
*/
#include <Servo.h> 
#include <SimpleTimer.h>

#define Pitch 9
#define Roll 10
#define Mode 11
#define SERVO 12
SimpleTimer timer;
Servo pitchServo;

int PitchMode = 0;
//0--PWM OUTPUT
//1--SERVO OUTPUT

int curState = 0;
int mode = 1;
//1--Pitch Control
//2--Locked Mode
//3--Dual Control

int pitch = 0;
int roll = 0;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initializing......"));
  pinMode(Roll, OUTPUT);
  pinMode(Mode, OUTPUT);
  pinMode(Pitch,OUTPUT);
 
  if (PitchMode==1){
    pitchServo.attach(SERVO);
    Serial.println(F("Pitch Output Set to Servo"));
  }
  else{
    Serial.println(F("Pitch Output Set to PWM"));
  }

  modeChange(1);
  
  Serial.println(F("---------------------------------------------------------------"));
  Serial.println(F("---------------------------------------------------------------"));
  Serial.println(F("    11           1111111111   11111111      11      11       11"));
  Serial.println(F("  11  11             11       11          11  11    111     111"));
  Serial.println(F(" 11    11            11       11111111   11    11   11 1   1 11"));
  Serial.println(F(" 11111111   11111    11       11111111   11111111   11 11 11 11"));
  Serial.println(F(" 11    11            11       11         11    11   11  111  11"));
  Serial.println(F(" 11    11            11       11111111   11    11   11   1   11"));
  Serial.println(F("---------------------------------------------------------------"));
  Serial.println(F("---------------------------------------------------------------"));
  Serial.println(F("pwnTest Initializing Finished"));
}

void loop() {
  
  int userInput;
  printMessage(curState);
  
  while(Serial.available()==0) { }
  userInput = Serial.parseInt();
  
  switch (curState){
    
    case 0://Status Output
      if (userInput ==0){
        displayStatus();  
        curState=0;
      }
      else if(userInput==1){
        curState=1;
      }
      else if(userInput==2){
        curState=2;
      }
      else if(userInput==3){
        curState=3;
      }
      else if(userInput==4){
        curState=4;
      }
      else{
        Serial.println(F("TRY AGAIN"));
      }
      break;
      
    case 1://Mode Set
      if(userInput<=3&&userInput>=1){
        mode = userInput;
        modeChange(mode);
        curState = 0;
      }
      else{
        Serial.println(F("INVALID INPUT"));
      }
      break;

    case 2://Roll Control
      if(mode ==2 ){
        Serial.print(F("ACCESS DENIED UNDER CURRENT MODE"));
        curState = 0;
      }
      else if(userInput<=255&&userInput>=0){
        roll = userInput;
        Serial.print(F("Roll Output Changed to "));Serial.println(roll);
        analogWrite(Roll,roll);
        curState = 0;
      }
      else{
        Serial.println(F("INVALID INPUT"));
      }
      break;

    case 3://Pitch Control
      if(mode == 2 || mode ==1){
        Serial.println(F("ACCESS DENIED UNDER CURRENT MODE"));
        curState = 0;
      }
      else if (PitchMode==0){
        if (userInput<=255&&userInput>=0){
          pitch = userInput;
          Serial.print(F("Pitch Output Changed to (PWM) "));Serial.println(pitch);
          curState = 0;
          analogWrite(Pitch,pitch);
        }
      }
      else if (PitchMode==1){
        if (userInput<=180&&userInput>=0){
          pitch = userInput;
          Serial.print(F("Pitch OutputChanged to (degree) "));Serial.println(pitch);
          curState = 0;
          pitchServo.write(pitch);
        }
      }
      else{
        Serial.println(F("INVALID INPUT"));
      }
      break;

    case 4://Pitch OUTPUT MODE SET
      if(userInput == 0){
        PitchMode = 0;
        pitchServo.detach();
        modeChange(1);
        Serial.println(F("PITCH OUTPUT SET TO PWM"));
      }
      else if(userInput == 1){
        PitchMode = 1;
        pitchServo.attach(SERVO);
        modeChange(1);
        Serial.println(F("PITCH OUTPUT SET TO SERVO"));
      }
      else{
        Serial.println(F("INVALID INPUT"));
      }
      curState = 0;
      break;
  }
}

void printMessage(int state){
  switch (state){
     case 0:
       Serial.println(F("-------------------------------------------------------------------------------"));
       Serial.println(F("Please Type in Command:\n0--DisplayCurrentOutput\n1--Change Mode\n2--Change Roll Output\n3--Change Pitch Output\n4--Change Pitch Mode(PWM/SERVO)"));
       break;
     case 2:
       Serial.println(F("Choose Roll input(0-255)"));
       break;
     case 3:
       Serial.println(F("Choose Pitch input(0-255 for pwm,0-180 for servo)"));
       break;
     case 1:
       Serial.println(F("Choose Mode:\n1--Pitch\n2--Lock\n3--Dual"));
       break;
     case 4:
       Serial.println(F("Choose Pitch Output Mode(All output will reset after change):\n0--PWM\n1--SERVO"));
       break;
  }
}

void modeChange(int mode){
  
  switch (mode){
    case 1:
      analogWrite(Mode,0);
      Serial.println(F("Mode Switched to 1 (Roll Control Mode)"));
      pitch = 0;
      roll = 0;
      clearOutput();
      break;
    case 2:
      analogWrite(Mode,20);
      Serial.println(F("Mode Switched to 2 (Lock Mode)"));      
      pitch = 0;
      roll = 0;
      clearOutput();
      break;
    case 3:
      analogWrite(Mode,26);
      Serial.println(F("Mode Switched to 3 (Pitch/Roll Control Mode)"));
      pitch = 0;
      roll = 0;
      clearOutput();
      break;
    default:
      Serial.println(F("Mode not found, No change committed"));
      break;
  }
}

void clearOutput(){
  if(PitchMode ==0){
    analogWrite(pitch,0);
    analogWrite(roll,0);
  }
  else{
    pitchServo.write(0);
    analogWrite(roll,0);
  }
}

void displayStatus(){
  switch (mode){
    case 1:
      Serial.println(F("Current Mode: Pitch Control"));
      break;
    case 2:
      Serial.println(F("Current Mode: Locked Mode"));
      break;
    case 3:
      Serial.println(F("Current Mode: Dual Control"));
      break;
  }
  Serial.print(F("Current Roll Output: "));Serial.println(roll);
  switch (PitchMode){
    case 0:
      Serial.print(F("Current Pitch Output (PWM): "));Serial.println(pitch);
      break;
    case 1:
      Serial.print(F("Current Pitch Output (SERVO,degree): "));Serial.println(pitch);
      break;
  } 
}

