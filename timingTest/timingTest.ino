/*
 * TIMING TEST FOR GIMBAL CONTROL
 * Xi Chen/A-Team
 * Currrently Controlling Mode Pin and Roll Pin with 500hz pwms
 * 
 * Roll output to pin 9 with 500hz at 8 bits
 * Pitch output to pin 10 with 500hz at 8 bits
 * No servo option with this test
 * No Mode output
*/
#include <Servo.h> 
#include <SimpleTimer.h>

#define Yaw 9
#define Pitch 10
#define BoardTwo 12

SimpleTimer timer;
Servo pitchServo;
Servo yawServo;

int curState = 0;


int pitch = 0;
int yaw = 0;
int timing = 0;
void setup() {
  
  Serial.begin(9600);
  Serial.println(F("Initializing......"));
  pinMode(BoardTwo,OUTPUT);
  digitalWrite(BoardTwo,LOW);
  pitchServo.attach(Pitch);
  yawServo.attach(Yaw);
  pitchServo.write(95);
  yawServo.write(90);
  
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
  Serial.println(F("TimingTest Initializing Finished"));
}

void loop() {
  
  int userInput;
  printMessage(curState);
  
  while(Serial.available()==0) { }
  userInput = Serial.parseInt();
  
  switch (curState){
    case 0:
      if (userInput == 0){
        displayStatus();
        curState = 0;
      }
      else if(userInput == 1){
        curState = 1;
      }
      else if(userInput == 2){
        curState = 2;
      }
      else if(userInput == 3){
        modeReset();
      }
      else{
        Serial.println(F("INVALID OPTION"));
      }
      break;

    case 1:
      if(userInput<=180 && userInput >=0){
        pitch = userInput;
        Serial.print(F("Pitch Servo set to: "));Serial.println(pitch);
        pitchServo.write(pitch);
        curState = 0;
      }
      else{
        Serial.println(F("Invalid Input"));
        curState = 0;
      }
      break;    
      
    case 2:
      if(userInput<=180 && userInput >=0){
        yaw = userInput;
        Serial.print(F("Yaw Servo set to: "));Serial.println(yaw);
        curState = 4;
      }
      else{
        Serial.println(F("Invalid Input"));
        curState = 0;
      }
      break;
      
    case 4:
      if(userInput<=20000 && userInput >=0){
        timing = userInput;
        Serial.print(F("Roll output time set to: "));Serial.println(timing);
        timingOutput(Yaw,yaw,timing);
        curState = 0;
      }
      else{
        Serial.println(F("Invalid Input"));
        curState = 0;
      }
      break;
  }
}

void printMessage(int state){
  switch (state){
     case 0:
       Serial.println(F("-------------------------------------------------------------------------------"));
       Serial.println(F("Please Type in Command:\n0--DisplayCurrentOutput\n1--Time Pitch Output\n2--Time Roll Output\n3--Mode Reset\n"));
       break;
     case 1:
       Serial.println(F("Choose Pitch output(0-255)"));
       break;
     case 2:
       Serial.println(F("Choose Roll output(0-255)"));
       break;
     case 4:
       Serial.println(F("Choose Roll output duration(0-20000ms)"));
       break;
  }
}

void timingOutput(int pin,int output,int timing){
  Serial.print(F("Start OUTPUTTING for "));Serial.print(timing);Serial.println(F(" ms"));
  if(pin == Pitch){
    pitchServo.write(output);
    delay(timing);
    pitchServo.write(95);
    Serial.println(F("End of OUTPUTTING Pitch, all output reset to zero"));
  }
  else{
    yawServo.write(output);
    delay(timing);
    yawServo.write(90);
    Serial.println(F("End of OUTPUTTING Yaw, all output reset to zero"));
  }
  clearOutput();
}

void clearOutput(){
    timing = 0;
    pitch = 0;
    yaw = 0;
}

void modeReset(){
  Serial.println(F("Resetting, Please wait for reset to complete..."));
  digitalWrite(BoardTwo,HIGH);
  delay(5000);
  digitalWrite(BoardTwo,LOW);
  Serial.println(F("Mode Reset Complete"));
}

void displayStatus(){
  Serial.print(F("Current Pitch Output: "));Serial.println(pitch);
  Serial.print(F("Current Roll Output (PWM): "));Serial.println(yaw);
}

