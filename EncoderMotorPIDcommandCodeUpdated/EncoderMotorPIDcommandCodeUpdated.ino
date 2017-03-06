/* a closed loop proportional control
*  js 20131008  updated 20160223
*/
#include <PinChangeInt.h>

#define  EncoderCountsPerRev (12.0)
//diameter of wheel is 3.25 in
#define  DistancePerRev       (0.85) //feet
#define  DistancePerTurn      (0.51) //feet
// these next two are the digital pins we'll use for the encoders
#define  EncoderMotor1  6
#define  EncoderMotor2 7
#define  RightBumper 4
#define  LeftBumper 5
//scaling gain values for error corrections
//#define  GAINmotor1  (3)
//#define  GAINmotor2 (2.46)
#define scale .75
#define  motor1     1
#define  motor2    2
#define pushButton 2
#define PushButtonState  (digitalRead(2))
// here we define the pwm and direction for Right Motor (1) and Left Motor (2)
#define  motor1PWM        3
#define  motor1Direction 12
#define  motor2PWM       11
#define  motor2Direction 13
#define pwm1    222//duty cycle on right wheel for straight line
#define pwm2    (int)pwm1*scale //duty cycle on left wheel for straight line
#define deadband1 222
#define deadband2 (int)deadband1*scale
///////////////////////////////////
volatile unsigned int leftEncoderCount = 0;
volatile unsigned int rightEncoderCount = 0; 
bool buttonState = 0;
bool rightBump = 0;
bool leftBump = 0;
bool buttonPrevious = 0;
bool button;
String directions[]={"F","L","F","R","F","L","F","L","F","R","F","R","F","L","F","R","F"};
int distances[]={3,90,1,90,1,90,1,90,4,90,1,90,1,90,1,90,3};

void setup(){
  // set stuff up
  Serial.begin(9600);
  Serial.print("Encoder Testing Program ");
  Serial.print("Now setting up the Right Encoder: Pin ");
  Serial.print(EncoderMotor2);
  Serial.println();
  pinMode(EncoderMotor2, INPUT_PULLUP); //use the internal pullup resistor
  // this next line setup the PinChange Interrupt 
  PCintPort::attachInterrupt(EncoderMotor2, indexLeftEncoderCount,CHANGE);
  // if you "really" want to know what's going on read the PinChange.h file :)
  /////////////////////////////////////////////////
  Serial.print("Now setting up the Left Encoder: Pin ");
  Serial.print(EncoderMotor1);
  Serial.println();
  pinMode(EncoderMotor1, INPUT_PULLUP); //use the internal pullup resistor
  //we could have set this up using just "pinMode(EncoderMotor2, INPUT_PULLUP);
  PCintPort::attachInterrupt(EncoderMotor1, indexRightEncoderCount,CHANGE);
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor1Direction, OUTPUT);
  pinMode(motor2Direction, OUTPUT);
  pinMode(pushButton, INPUT);
  pinMode(RightBumper, INPUT);
  pinMode(LeftBumper, INPUT);
} /////////////// end of setup ////////////////////////////////////

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
/////////////////////// loop() ////////////////////////////////////
void loop()
{
  Serial.println("Waiting for a button push to start");
  button = digitalRead(pushButton);
  if (button != buttonPrevious) {
    if (button == 1){
      buttonState = !buttonState;
    }
  }
  buttonPrevious = button;
//delay(1000);  // wait a second before going into the motion loop
  if(buttonState){
    Serial.println("Button pushed!");
    for (int i=0; i < 17; i++){  
      Serial.println(i);
      drive(directions[i],distances[i]);
      delay(1000);
    } 
    exit(0);
  }
}
//////////////////////////////// end of loop() /////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
void drive(String dirct, int dist) // direction gives forward left or right, distance is how many units forward, or 90 deg left or right
{
    int countsDesired;
    int CMDleft, CMDright;
    int errorLeft;
    int errorRight;
    if (dirct == "F"){
      countsDesired = (int) ((dist / DistancePerRev) * EncoderCountsPerRev);
    }
    else if (dirct == "L" || dirct == "R") {
      countsDesired = (int) ((DistancePerTurn / DistancePerRev) * EncoderCountsPerRev); //12 steps per rev r_chasis=3.375 in, d_wheel=3.25in
    }
//  we make the errors non-zero so our first test gets us into the loop
    errorLeft = countsDesired;
    errorRight = countsDesired;
    while(errorLeft > 0 || errorRight > 0){
//    Serial.println();
//    Serial.print("Counts Desired: ");
//    Serial.println(countsDesired);
//    Serial.print("Error Right: ");
//    Serial.print(errorRight);
//    Serial.print(" Error Left: ");
//    Serial.println(errorLeft);
      CMDleft = computeCommand(countsDesired, errorLeft, pwm2,deadband2);
      CMDright = computeCommand(countsDesired, errorRight, pwm1,deadband1);
      if (dirct == "F"){
        motor(motor1, CMDright);
        motor(motor2, CMDleft);
      }
      else if (dirct ==  "L"){
        motor(motor1, CMDright);
        motor(motor2, -CMDleft);
      }
      else if (dirct ==  "R"){
        motor(motor1, -CMDright);
        motor(motor2, CMDleft);
      }
      errorRight = countsDesired - rightEncoderCount; 
      errorLeft = countsDesired - leftEncoderCount; 
//    Serial.println();
//    Serial.print(errorRight);
//    Serial.print(",");
//    Serial.println(errorLeft);
//    need to convert to Arduino Speak
//    printf("eL= %d, eR= %d,  cL=%d, cR=%d\n",errorLeft, errorRight, cmdLeft, cmdRight);
      rightBump = digitalRead(RightBumper);
      leftBump = digitalRead(LeftBumper);
      if (rightBump){
        motor(motor1, pwm1);
        motor(motor2,-pwm2);
        delay(200);
        rightBump = 0;
      }
      if (leftBump) {
        motor(motor1,-pwm1);
        motor(motor2,pwm2);
        delay(200);
        leftBump = 0;
      }
    }
    countsDesired = 0;
    rightEncoderCount = 0;
    leftEncoderCount = 0;
    errorRight = 0;
    errorLeft = 0;
    CMDright = 0;
    CMDleft = 0;
    return;    
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
  Serial.print(" entering motor");
  Serial.print(m);
  Serial.print("\t");
  Serial.println(pwm);
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
///////////////////////////////////////////////////////////////




