#include <PinChangeInt.h>
#define  EncoderMotorLeft  7
#define  EncoderMotorRight 6
#define  EncoderCountsPerRev (12.0)
//diameter of wheel is 3.25 in
#define  DistancePerRev       (0.85) //feet per wheel revolution
#define  DistancePerTurn      (0.33) //feet per 90 deg
#define  scale .79
#define  motor1     1
#define  motor2    2
#define  motor1PWM        3
#define  motor1Direction 12
#define  motor2PWM       11
#define  motor2Direction 13
#define pwm1    240//duty cycle on right wheel for straight line
#define pwm2    (int)pwm1*scale //duty cycle on left wheel for straight line
#define deadband1 222
#define deadband2 (int)deadband1*scale

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
    int countsDesired;
    int CMDleft, CMDright;
    int errorLeft;
    int errorRight;
    countsDesired = (int) (DistancePerTurn * EncoderCountsPerRev / DistancePerRev); //12 steps per rev r_chasis=3.375 in, d_wheel=3.25in
    errorLeft = countsDesired;
    errorRight = countsDesired;
    while(errorLeft > 0 || errorRight > 0){
      CMDright = computeCommand(countsDesired, errorRight, pwm1,deadband1);
      CMDleft = computeCommand(countsDesired, errorLeft, pwm2,deadband2);
      motor(motor1, -CMDright);
      motor(motor2, CMDleft);
      errorRight = countsDesired - rightEncoderCount; 
      errorLeft = countsDesired - leftEncoderCount;
      Serial.println();
      Serial.print(errorLeft);
      Serial.print(",");
      Serial.print(errorRight);
    }
    motor(motor1, 0);
    motor(motor2, 0);
    Serial.println("1 turn");
    exit(0);
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
/////////////////////////////////////////////////////////////////
int computeCommand(int count, int err, int PWM, int deadband)
//  gain, and error, both are integer values
{
//    Serial.println();
//    Serial.print(err);
//    Serial.print("/");
//    Serial.print(count);
//    Serial.print("=");
//    Serial.println((int)(err/count));
    int cmdDir = (PWM*err/count);
    if (err <= 0) {
      cmdDir = 0;
    }
//    Serial.print("Initial: ");
//    Serial.print(cmdDir);
    if (cmdDir  > PWM){ 
      cmdDir = PWM;
    }
    else if(cmdDir < deadband){
      cmdDir = deadband;
    }
//    Serial.print(" Adjustments: ");
//    Serial.println(cmdDir);
    return(cmdDir);
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

