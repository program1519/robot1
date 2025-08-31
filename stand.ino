#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <Servo.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
Servo servoD3; 
Servo servoD4; 
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
  {90, 140, 40}, 
  {90, 80, 20}, 
  {90, 140, 40}, 
  {90, 80, 20}, 
  {90, 140, 40}, 
  {90, 80, 20}  
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

void setStandPose(){
  for(int i=0;i<6;i++){
    int coxa = basePose[i].coxa;
    int femur = basePose[i].femur;
    int tibia = basePose[i].tibia;

    if(i==1 || i==3 || i==5){
      femur = 180 - femur;
      tibia = 180 - tibia;
    }

    switch(i){
      case 0: // Right-front
        setPCA(2, coxa); // Coxa
        setPCA(1, femur); // Femur
        setPCA(0, tibia); // Tibia
        break;
      case 1: // Left-front
        setPCA(5, coxa);
        setPCA(4, femur);
        setPCA(3, tibia);
        break;
      case 2: // Right-middle
        setPCA(11, coxa);
        setPCA(10, femur);
        setPCA(9, tibia);
        break;
      case 3: // Left-middle
        setPCA(8, coxa);
        setPCA(7, femur);
        setPCA(6, tibia);
        break;
      case 4: // Right-back
        setPCA(15, coxa);
        setD4(femur);
        setD3(tibia);
        break;
      case 5: // Left-back
        setPCA(14, coxa);
        setPCA(13, femur);
        setPCA(12, tibia);
        break;
    }
  }
}

void setup(){
  pwm.begin();
  pwm.setPWMFreq(PWM_FREQ);

  servoD3.attach(PIN_SERVO_D3);
  servoD4.attach(PIN_SERVO_D4);
}

void loop(){
  delay(1000);
}
