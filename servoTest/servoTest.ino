#include <Servo.h>

Servo pitchServo;
Servo yawServo;
Servo modeServo;

#define PITCH_PIN         (10)
#define YAW_PIN           (9)// we only have two pins available so sometimes we have to change our mapping
#define MODE_PIN          (9)

#define PITCH_ENABLED
#define YAW_ENABLED
#define MODE_ENABLED

// prever yaw to mode if possible
#ifdef YAW_ENABLED
#undef MODE_ENABLED
#endif

enum GimbalMode
{
  GM_YAW_LOCKED,
  GM_ALL_LOCKED,
  GM_UNLOCKED
};

int ModeToDelayUs(int mode)
{
  switch(mode) {
    case GM_YAW_LOCKED: return 1100;// us
    case GM_ALL_LOCKED: return 1500;// us
    case GM_UNLOCKED: return 1900;// us
    default: return 0;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("--simple gimbal controller--");

#ifdef PITCH_ENABLED
  pitchServo.attach(PITCH_PIN);
  Serial.print("pitch enabled on pin: ");
  Serial.println(PITCH_PIN);
#endif
#ifdef YAW_ENABLED
  yawServo.attach(YAW_PIN);
  Serial.print("yaw enabled on pin: ");
  Serial.println(YAW_PIN);
#endif
#ifdef MODE_ENABLED
  modeServo.attach(MODE_PIN);
  Serial.print("mode enabled on pin: ");
  Serial.println(MODE_PIN);
#endif
}

void loop() {
  if(Serial.available()) {
    int cmd = Serial.read();
    switch(cmd) {
      case 'm': case 'M':
        ChangeMode(Serial.parseInt());
        break;
      case 'p': case 'P':
        ChangePitch(Serial.parseInt());
        break;
      case 'y': case 'Y':
        ChangeYaw(Serial.parseInt());
        break;
      case 'h': case 'H':
        Serial.println(
          "help: \n"
          "\tchange mode: m<0|1|2>\n"
          "\tchange pitch: p<0-180>\n"
          "\tchange yaw: y<0-180>\n"
          "\tshow help: h\n"
        );
        break;
      case -1:
        // no data
        break;
      default:
        Serial.print("invalid cmd: ");
        Serial.println(cmd);
        break;
    }
  }
  delay(15);
}

void ChangeMode(int mode)
{
#ifdef MODE_ENABLED
  int modeDelay = ModeToDelayUs(mode);
  if(modeDelay <= 0) {
    Serial.print("invalid mode: ");
    Serial.println(mode);
    return;
  }
  modeServo.writeMicroseconds(modeDelay);
  Serial.print("switched to mode: ");
  Serial.println(mode);
#else
  Serial.println("mode disabled");
#endif
}

bool CheckRange(int val)
{
  if(val < 0 || val > 180) {
    Serial.print("value out of range (0-180): ");
    Serial.println(val);
    return false;
  }
  return true;
}

void ChangePitch(int pitch)
{
#ifdef PITCH_ENABLED
  if(!CheckRange(pitch)) {
    return;
  }
  pitchServo.write(pitch);
  Serial.print("switched pitch to: ");
  Serial.println(pitch);
#else
  Serial.println("pitch disabled");
#endif
}

void ChangeYaw(int yaw)
{
#ifdef YAW_ENABLED
  if(!CheckRange(yaw)) {
    return;
  }
  yawServo.write(yaw);
  Serial.print("switched yaw to: ");
  Serial.println(yaw);
#else
  Serial.println("yaw disabled");
#endif
}

