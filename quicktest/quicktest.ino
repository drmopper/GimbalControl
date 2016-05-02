#include <SimpleTimer.h>

#define ModePin 10
#define BoardOne 12
#define DualMode 1
#define PitchMode 2


SimpleTimer timer;
int Output = DualMode;

void setup() {
  Serial.begin(9600);
  pinMode(ModePin,OUTPUT);
  pinMode(BoardOne,INPUT);
  timer.setInterval(18,pwmOutput);
}

void loop() {

  timer.run();
}

void pwmOutput(){
  int boardOneInput = digitalRead(BoardOne);
  if(boardOneInput == HIGH){
    digitalWrite(10,HIGH);
    delay(PitchMode);
    digitalWrite(10,LOW);
  }
  else{
    digitalWrite(10,HIGH);
    delay(DualMode);
    digitalWrite(10,LOW);
  }
}

