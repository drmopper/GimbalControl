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

#define SERVO_YAW_HALT    (90)
#define SERVO_YAW_CW      (138)
#define SERVO_YAW_CCW     (48)
#define ROTATE_TIME       (10120)// ms
#define ROTATE_DEG_TIME   (ROTATE_TIME / 360)// ms 1 deg msb

#define YawCCWCycleTime 10120
#define YawCWCycleTime 10120

#define SERVO_MIN_PITCH   (-90)
#define SERVO_MAX_PITCH   (45)
#define SERVO_MIN_OUTPUT  (46)// the min value we can output to our server to get SERVO_MIN_PITCH
#define SERVO_MAX_OUTPUT  (119)// same as SERVO_MIN_OUTPUT but for SERVO_MAX_PITCH

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

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1000);
  
  pitchServo.attach(PitchPin);
  setPitch(0);

  yawServo.attach(YawPin);
  setYaw(0);
}

void loop() {
  (void)readInput();
}

float clamp(float val, float minVal, float maxVal) {
  if(val < minVal) return minVal;
  if(val > maxVal) return maxVal;
  return val;
}

int clamp(int val, int minVal, int maxVal) {
  if(val < minVal) return minVal;
  if(val > maxVal) return maxVal;
  return val;
}

bool readInput(){
  // read interface header
  byte rsize = Serial.readBytes(ifBuff, HEADER_SIZE);
  if (rsize == 0) {
    //Serial.println("Timeout");
    return false;
  } else if(rsize < HEADER_SIZE) {
    Serial.print("Invalid Header Read: ");
    Serial.println((int)rsize);
    return false;
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
    return false;
  }

  // parse the packets based on the id
  switch(id) {
    case CMD_SET_ANGLE: processAngles(); return true;
    case CMD_SET_MODE: processMode(); return true;
    default:
      Serial.print("Invalid Cmd Id: ");
      Serial.println((int)id);
      break;
  }
  return false;
}

void processAngles() {
  float pitch = *(float*)ifBuff;
  float yaw = *(float*)&ifBuff[sizeof(float)];
  pitch = clamp(pitch, -90.0f, 90.0f);
  yaw = clamp(yaw, 0.0f, 360.0f);
  int inputPitch = (int)pitch;
  int inputYaw = (int)yaw;
  Serial.print("Set Angle: (");
  Serial.print(inputPitch);
  Serial.print(", ");
  Serial.print(inputYaw);
  Serial.println(")");
  setPitch(inputPitch);
  setYaw(inputYaw);
}

void processMode() {
  switch((gMode = ifBuff[0])) {
    case MODE_RESET:
      resetMode();
      break;
    default:
      gMode = MODE_INVALID;
      Serial.println("Invalid Mode");
      break;
  }
}

void resetMode(){
  Serial.println(F("Resetting, Please wait for reset to complete..."));
  digitalWrite(BoardTwo, HIGH);
  delay(5000);
  digitalWrite(BoardTwo, LOW);
  Serial.println(F("Mode Reset Complete"));
}

void setPitch(int pitch) {
  // clamp our pitch to our servos range
  pitch = clamp(pitch, SERVO_MIN_PITCH, SERVO_MAX_PITCH);
  int output = map(pitch, SERVO_MIN_PITCH, SERVO_MAX_PITCH, SERVO_MIN_OUTPUT, SERVO_MAX_OUTPUT);
  pitchServo.write(output);
}

int deltaToTarget(int current, int target) {
  int sign = 1;
  int delta = 0;
  if(target > current)
  {
      delta = target - current;
      sign = 1;
  }
  else
  {
      delta = current - target;
      sign = -1;
  }
  if(delta > 180)
  {
      delta = 360 - delta;
      sign = -sign;
  }
  return delta * sign;
}

void setYaw(int yaw) {
  static int currentYaw = 0;
  int delta = deltaToTarget(currentYaw, yaw);
  if(delta != 0) {
    yawServo.write(delta < 0 ? YawCCW : YawCW);
    delay(abs(delta) * ROTATE_DEG_TIME);
    yawServo.write(YawHalt);
  } else {
    yawServo.write(YawHalt);
  }
  currentYaw = yaw;
}
//
//void turnYaw() {
//  if(inputYaw>=currentYaw){
//    if((inputYaw-currentYaw)<=180){
//      turnYawCW(inputYaw-currentYaw);
//    }
//    else{
//      turnYawCCW(360-inputYaw+currentYaw);
//    }
//  }
//  else{
//    if((currentYaw-inputYaw)<=180){
//      turnYawCCW(currentYaw-inputYaw);
//    }
//    else{
//      turnYawCW(360-(currentYaw-inputYaw));
//    }
//  }
//}
//
//void turnYawCW(int degree){
//  if((currentYaw + degree)>360){
//    resetMode();
//    yawServo.write(YawCW);
//    delay((currentYaw+degree-360)*YawCWCycleTime/360);
//    yawServo.write(YawHalt);
//  }
//  else{
//    yawServo.write(YawCW);
//    delay(degree*YawCWCycleTime/360);
//    yawServo.write(YawHalt);
//  }
//}
//
//void turnYawCCW(int degree){
//  if((currentYaw-degree)<0){
//    resetMode();
//    yawServo.write(YawCCW);
//    delay((degree-currentYaw)*YawCCWCycleTime/360);
//    yawServo.write(YawHalt);
//  }
//  else{
//    yawServo.write(YawCCW);
//    delay(degree*YawCCWCycleTime/360);
//    yawServo.write(YawHalt);
//  }
//}
//

