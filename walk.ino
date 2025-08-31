#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Servo servoD3; // servo D3
Servo servoD4; // servo D4
const int PIN_SERVO_D3 = 3;
const int PIN_SERVO_D4 = 4;

const int SERVOMIN = 500;
const int SERVOMAX = 2500;
const int PWM_FREQ = 50;

struct LegAngles {
  int coxa;
  int femur;
  int tibia;
};

LegAngles basePose[6] = {
  {90, 140, 40}, // Right-front
  {90, 80, 20},  // Left-front
  {90, 140, 40}, // Right-middle
  {90, 80, 20},  // Left-middle
  {90, 140, 40}, // Right-back
  {90, 80, 20}   // Left-back
};

uint16_t angleToTick(int angle){
  if(angle<0) angle=0;
  if(angle>180) angle=180;
  int pulse_us = map(angle,0,180,SERVOMIN,SERVOMAX);
  float ticks = (pulse_us*4096.0)/(1000000.0/PWM_FREQ);
  if(ticks<0) ticks=0;
  if(ticks>4095) ticks=4095;
  return (uint16_t)ticks;
}

void setPCA(int ch, int ang){
  if(ch<0||ch>=16) return;
  pwm.setPWM(ch,0,angleToTick(ang));
}

void setD3(int ang){ servoD3.write(ang); }
void setD4(int ang){ servoD4.write(ang); }

void setLegPose(int legIndex, int coxa, int femur, int tibia){
  if(legIndex==1 || legIndex==3 || legIndex==5){ 
    femur = 180 - femur;
    tibia = 180 - tibia;
  }
  switch(legIndex){
    case 0: setPCA(2, coxa); setPCA(1, femur); setPCA(0, tibia); break; 
    case 1: setPCA(5, coxa); setPCA(4, femur); setPCA(3, tibia); break; 
    case 2: setPCA(11, coxa); setPCA(10, femur); setPCA(9, tibia); break; 
    case 3: setPCA(8, coxa); setPCA(7, femur); setPCA(6, tibia); break; 
    case 4: setPCA(15, coxa); setD4(femur); setD3(tibia); break;         
    case 5: setPCA(14, coxa); setPCA(13, femur); setPCA(12, tibia); break; 
  }
}

void setStandPose(){
  for(int i=0;i<6;i++){
    setLegPose(i, basePose[i].coxa, basePose[i].femur, basePose[i].tibia);
  }
}

// ยกขา -> เหวี่ยงไปข้างหน้า -> วางลง
void walkForwardStep(int leg){
  int coxa = basePose[leg].coxa;
  int femur = basePose[leg].femur;
  int tibia = basePose[leg].tibia;

  // 1) ยกขึ้น
  setLegPose(leg, coxa, femur-20, tibia-20);
  delay(400);

  // 2) ก้าวไปข้างหน้า
  setLegPose(leg, coxa+20, femur-20, tibia-20);
  delay(400);

  // 3) วางลง
  setLegPose(leg, coxa+20, femur, tibia);
  delay(400);
}

void setup(){
  Serial.begin(115200);
  while(!Serial);

  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);

  servoD3.attach(PIN_SERVO_D3);
  servoD4.attach(PIN_SERVO_D4);

  delay(500);
  setStandPose();
}

void loop(){

  walkForwardStep(0); 
  walkForwardStep(3); 
  walkForwardStep(4); 
  walkForwardStep(1); 
  walkForwardStep(2); 
  walkForwardStep(5); 
}
