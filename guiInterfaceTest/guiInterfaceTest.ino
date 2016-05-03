
#define MAX_BUFF (64)
#define HEADER_SIZE (2)
byte _buff[MAX_BUFF];

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
byte _mode = MODE_INVALID;

float Clamp(float val, float minVal, float maxVal) {
  if(val < minVal) return minVal;
  if(val > maxVal) return maxVal;
  return val;
}

float _pitch = 0.0f;
float _yaw = 0.0f;

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(1000);
}

void loop() {
  byte rsize = Serial.readBytes(_buff, HEADER_SIZE);
  if (rsize == 0) {
    //Serial.println("Timeout");
    return;
  } else if(rsize < HEADER_SIZE) {
    Serial.print("Invalid Header Read: ");
    Serial.println((int)rsize);
    return;
  }
  byte id = _buff[0];
  byte dsize = _buff[1];
  if(dsize > MAX_BUFF - HEADER_SIZE) {
     Serial.print("Invalid Size: ");
     Serial.println((int)dsize);
  }
  rsize = Serial.readBytes(_buff, dsize);
  if(rsize < dsize) {
    Serial.print("Invalid Data Read: ");
    Serial.println((int)rsize);
    return;
  }
  switch(id) {
    case CMD_SET_ANGLE: ParseAndSetAngle(); break;
    case CMD_SET_MODE: ParseAndSetMode(); break;
    default: Serial.println("Invalid Cmd Id"); break;
  }
}

void ParseAndSetAngle() {
  // parse from the packet
  _pitch = *(float*)_buff;
  _yaw = *(float*)&_buff[sizeof(float)];
  // ensure range
  _pitch = Clamp(_pitch, -90.0f, 90.0f);
  _yaw = Clamp(_yaw, 0.0f, 360.0f);
  // report success
  Serial.print("Set Angles: (");
  Serial.print(_pitch);
  Serial.print(", ");
  Serial.print(_yaw);
  Serial.println(")");
}

void ParseAndSetMode() {
  _mode = _buff[0];
  switch(_mode) {
    case MODE_RESET: Serial.println("Set Mode: RESET"); break;
    default:
      _mode = MODE_INVALID;
      Serial.println("Invalid Mode");
      break;
  }
}

