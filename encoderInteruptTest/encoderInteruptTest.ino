#include <PinChangeInt.h>
#define  EncoderMotorLeft  7
#define  EncoderMotorRight 6

#define  scale .79
#define  motor1     1
#define  motor2    2
#define  motor1PWM        3
#define  motor1Direction 12
#define  motor2PWM       11
#define  motor2Direction 13
#define pwm1    222//duty cycle on right wheel for straight line
#define pwm2    (int)pwm1*scale //duty cycle on left wheel for straight line

volatile unsigned int leftEncoderCount = 0;
volatile unsigned int rightEncoderCount = 0; 
void setup() {
   // set stuff up
  Serial.begin(9600);
  Serial.print("Encoder Testing Program ");
  Serial.print("Now setting up the Left Encoder: Pin ");
  Serial.print(EncoderMotorLeft);
  Serial.println();
  pinMode(EncoderMotorLeft, INPUT);     //set the pin to input
  digitalWrite(EncoderMotorLeft, HIGH); //use the internal pullup resistor
  // this next line setup the PinChange Interrupt 
  PCintPort::attachInterrupt(EncoderMotorLeft, indexLeftEncoderCount,CHANGE);
  // if you "really" want to know what's going on read the PinChange.h file :)
  /////////////////////////////////////////////////
  Serial.print("Now setting up the Right Encoder: Pin ");
  Serial.print(EncoderMotorRight);
  Serial.println();
  pinMode(EncoderMotorRight, INPUT);     //set the pin to input
  digitalWrite(EncoderMotorRight, HIGH); //use the internal pullup resistor
  //we could have set this up using just "pinMode(EncoderMotorRight, INPUT_PULLUP);
  PCintPort::attachInterrupt(EncoderMotorRight, indexRightEncoderCount,CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println();
  Serial.print("left: ");
  Serial.print(leftEncoderCount);
  Serial.print(" right: ");
  Serial.print(rightEncoderCount);
//  Serial.print("Difference: ");
//  Serial.print(rightEncoderCount - leftEncoderCount);
//  motor(motor1, 0);
//  motor(motor2, 0);
  
}

//////////////////////////////////////////////////////////
void indexLeftEncoderCount()
{
  leftEncoderCount++;
}
//////////////////////////////////////////////////////////
void indexRightEncoderCount()
{
  rightEncoderCount++;
}

///////////////////////////////////////////////////////////////
void motor(int m, int pwm){
//  Serial.print(" entering motor");
//  Serial.print(m);
//  Serial.print("\t");
//  Serial.println(pwm);
  
  if(m == motor1){
    if(pwm > 0){
      digitalWrite(motor1Direction, HIGH);
      analogWrite(motor1PWM, pwm);
    }
    else if(pwm < 0){
      digitalWrite(motor1Direction, LOW);
      analogWrite(motor1PWM, -pwm);
      }
    else{
      analogWrite(motor1PWM, 0);
    }
  }
  if(m == motor2){
    if(pwm > 0){
      digitalWrite(motor2Direction, HIGH);
      analogWrite(motor2PWM, pwm);
    }
    else if(pwm < 0){
      digitalWrite(motor2Direction, LOW);
      analogWrite(motor2PWM, -pwm);
    }
    else {
      analogWrite(motor2PWM, 0);
    }
  }
}

