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

#define YawPin 9
#define PitchPin 10
#define BoardTwo 12

#define YawHalt 90
#define YawCCW 48
#define YawCW 138
#define YawCCWCycleTime 10120
#define YawCWCycleTime 10120

#define MAX_BUFF (64)
#define HEADER_SIZE (2)
byte ifBuff[MAX_BUFF];

enum CmdId
{
  CMD_SET_ANGLE = 'A',
  CMD_SET_MODE = 'M'
};

enum GimbalMode
{
  MODE_INVALID = 0,
  MODE_RESET = 'R'
};
byte gMode = MODE_INVALID;

Servo pitchServo;
Servo yawServo;

const int pitchMap[] PROGMEM= {46,47,47,48,48,49,49,50,50,51,51,52,52,53,54,54,55,55,56,56,57,57,58,58,59,60,60,61,61,62,62,63,63,64,64,65,65,66,66,67,68,68,69,69,70,70,71,71,72,72,73,74,74,75,75,76,76,77,77,78,78,79,80,80,81,81,82,82,83,83,84,84,85,85,86,87,87,88,88,89,89,90,90,91,91,92,93,93,94,94,95,95,96,96,97,97,98,98,99,100,100,101,101,102,102,103,103,104,104,105,105,106,107,107,108,108,109,109,110,110,111,111,112,113,113,114,114,115,115,116,116,117,117,118,118,119};
int currentPitch;
int currentYaw;
int inputPitch;
int inputYaw;
boolean shouldReset;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1000);
  
  pitchServo.attach(PitchPin);
  yawServo.attach(YawPin);
  
  currentPitch = 0;//zero points straight down
  currentYaw = 0;//zero points straight forward
  inputPitch = 0;
  inputYaw = 0;
  
  pitchServo.write(pitchMap[currentPitch]);//set pitch position to 90
  yawServo.write(YawHalt);//halt any yaw movement
}

void loop() {
  //wait for input and read input from serial
  //extract info and parse to Int to inputPitch, inputYaw, and shouldReset
  //using readInput()
  readInput();
  turnPitch();
  turnYaw();
  currentPitch = inputPitch;
  currentYaw = inputYaw;
  if(shouldReset){
    resetMode();
  }
  shouldReset = false;
}

float clamp(float val, float minVal, float maxVal) {
  if(val < minVal) return minVal;
  if(val > maxVal) return maxVal;
  return val;
}

void readInput(){
  // read interface header
  byte rsize = Serial.readBytes(ifBuff, HEADER_SIZE);
  if (rsize == 0) {
    //Serial.println("Timeout");
    return;
  } else if(rsize < HEADER_SIZE) {
    Serial.print("Invalid Header Read: ");
    Serial.println((int)rsize);
    return;
  }

  // parse the header and read the data
  byte id = ifBuff[0];
  byte dsize = ifBuff[1];
  if(dsize > MAX_BUFF) {
     Serial.print("Invalid Size: ");
     Serial.println((int)dsize);
  }
  rsize = Serial.readBytes(ifBuff, dsize);
  if(rsize < dsize) {
    Serial.print("Invalid Data Read: ");
    Serial.println((int)rsize);
    return;
  }

  // parse the packets based on the id
  switch(id) {
    case CMD_SET_ANGLE: parseAngles(); break;
    case CMD_SET_MODE: parseMode(); break;
    default:
      Serial.print("Invalid Cmd Id: ");
      Serial.println((int)id);
      break;
  }
}

void parseAngles() {
  float pitch = *(float*)ifBuff;
  float yaw = *(float*)&ifBuff[sizeof(float)];
  pitch = clamp(pitch, -90.0f, 90.0f);
  yaw = clamp(yaw, 0.0f, 360.0f);
  inputPitch = (int)pitch;
  inputYaw = (int)yaw;
  Serial.print("Set Angle: (");
  Serial.print(inputPitch);
  Serial.print(", ");
  Serial.print(inputYaw);
  Serial.println(")");
}

void parseMode() {
  switch((gMode = ifBuff[0])) {
    case MODE_RESET:
      shouldReset = true;
      Serial.println("RESET");
      break;
    default:
      gMode = MODE_INVALID;
      Serial.println("Invalid Mode");
      break;
  }
}

void resetMode(){
  Serial.println(F("Resetting, Please wait for reset to complete..."));
  digitalWrite(BoardTwo,HIGH);
  delay(5000);
  digitalWrite(BoardTwo,LOW);
  Serial.println(F("Mode Reset Complete"));
}

void turnPitch(){
  if(inputPitch!=currentPitch){
    pitchServo.write(pitchMap[inputPitch]);
  }
}

void turnYaw(){
  if(inputYaw>=currentYaw){
    if((inputYaw-currentYaw)<=180){
      turnYawCW(inputYaw-currentYaw);
    }
    else{
      turnYawCCW(360-inputYaw-currentYaw);
    }
  }
  else{
    if((currentYaw-inputYaw)<=180){
      turnYawCCW(currentYaw-inputYaw);
    }
    else{
      turnYawCW(360-inputYaw-currentYaw);
    }
  }
}

void turnYawCW(int degree){
  if((currentYaw + degree)>360){
    resetMode();
    yawServo.write(YawCW);
    delay((currentYaw+degree-360)*YawCWCycleTime*1000/360);
    yawServo.write(YawHalt);
  }
  else{
    yawServo.write(YawCW);
    delay(degree*YawCWCycleTime*1000/360);
    yawServo.write(YawHalt);
  }
}

void turnYawCCW(int degree){
  if((currentYaw-degree)<0){
    resetMode();
    yawServo.write(YawCCW);
    delay((degree-currentYaw)*YawCCWCycleTime*1000/360);
    yawServo.write(YawHalt);
  }
  else{
    yawServo.write(YawCCW);
    delay(degree*YawCCWCycleTime*1000/360);
    yawServo.write(YawHalt);
  }
}


