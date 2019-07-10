#include "vex.h"
/**************************************************/
//user editable constants

//motor ports
const int left_front = PORT5;
const int left_rear = PORT6;
const int right_front = PORT3;
const int right_rear = PORT4;

//distance constants
const double degree_constant = 2.3; //ticks per degree
const int tile_constant = 545; //ticks per tile


/**************************************************/
//advanced tuning (PID and slew)

//slew control (autonomous only)
const int accel_step = 8; //smaller number = more slew
const int deccel_step = 200; //200 = no slew

//straight driving constants
const double driveKP = .3;
const double driveKD = .5;

//turning constants
const double turnKP = .8;
const double turnKD = 3;


/**************************************************/
//edit below with caution!!!


#define MAX 100;

static int driveMode = 1;
static int driveTarget = 0;
static int turnTarget = 0;
static int maxSpeed = MAX;


//motors
motor left1(left_front, gearSetting::ratio18_1, 0);
motor left2(left_rear, gearSetting::ratio18_1, 0);
motor right1(right_front, gearSetting::ratio18_1, 1);
motor right2(right_rear, gearSetting::ratio18_1, 1);

/**************************************************/
//basic control
void left(int vel){
  vel *= 120;
  left1.spin(directionType::fwd, vel, voltageUnits::mV);
  left2.spin(directionType::fwd, vel, voltageUnits::mV);
}

void right(int vel){
  vel *= 120;
  right1.spin(directionType::fwd, vel, voltageUnits::mV);
  right2.spin(directionType::fwd, vel, voltageUnits::mV);
}

void reset(){
  maxSpeed = MAX;
  driveTarget = 0;
  turnTarget = 0;
  left1.resetRotation();
  left2.resetRotation();
  right1.resetRotation();
  right2.resetRotation();
  left(0);
  right(0);
}

int drivePos(){
  return (left1.rotation(rotationUnits::deg) + left2.rotation(rotationUnits::deg))/2;
}

/**************************************************/
//slew control
static int leftSpeed = 0;
static int rightSpeed = 0;

void leftSlew(int leftTarget){
  int step;

  if(abs(leftSpeed) < abs(leftTarget))
    step = accel_step;
  else
    step = deccel_step;

  if(leftTarget > leftSpeed + step)
    leftSpeed += step;
  else if(leftTarget < leftSpeed - step)
    leftSpeed -= step;
  else
    leftSpeed = leftTarget;

  left(leftSpeed);
}

//slew control
void rightSlew(int rightTarget){
  int step;

  if(abs(rightSpeed) < abs(rightTarget))
    step = accel_step;
  else
    step = deccel_step;

  if(rightTarget > rightSpeed + step)
    rightSpeed += step;
  else if(rightTarget < rightSpeed - step)
    rightSpeed -= step;
  else
    rightSpeed = rightTarget;

  right(rightSpeed);
}

/**************************************************/
//feedback
bool isDriving(){
  static int count = 0;
  static int last = 0;
  static int lastTarget = 0;

  int curr = left2.rotation(rotationUnits::deg);

  int target = turnTarget;
  if(driveMode == 1)
    target = driveTarget;

  if(abs(last-curr) < 3)
    count++;
  else
    count = 0;

  if(target != lastTarget)
    count = 0;

  lastTarget = target;
  last = curr;

  //not driving if we haven't moved
  if(count > 4)
    return false;
  else
    return true;

}
/**************************************************/
//drive modifiers
void setSpeed(int speed){
  maxSpeed = speed;
}

/**************************************************/
//autonomous functions
void driveAsync(double sp){
  sp *= tile_constant;
  reset();
  driveTarget = sp;
  driveMode = 1;
}

void turnAsync(double sp){
  sp *= degree_constant;
  reset();
  turnTarget = sp;
  driveMode = 0;
}

void drive(double sp){
  driveAsync(sp);
  task::sleep(450);
  while(isDriving()) task::sleep(20);
}

void turn(double sp){
  turnAsync(sp);
  task::sleep(450);
  while(isDriving()) task::sleep(20);
}

void slowDrive(double sp, double dp){
  driveAsync(sp);

  if(sp > 0)
    while(drivePos() < dp) task::sleep(20);
  else
    while(drivePos() > dp) task::sleep(20);

  setSpeed(60);
  while(isDriving()) task::sleep(20);
}

/**************************************************/
//task control
int driveTask(){
  int prevError = 0;

  while(1){
    task::sleep (20);

    if(driveMode != 1)
      continue;

    int sp = driveTarget;

    double kp = driveKP;
    double kd = driveKD;

    //read sensors
    int sv = left2.rotation(rotationUnits::deg);

    //speed
    int error = sp-sv;
    int derivative = error - prevError;
    prevError = error;
    int speed = error*kp + derivative*kd;

    if(speed > maxSpeed)
      speed = maxSpeed;
    if(speed < -maxSpeed)
      speed = -maxSpeed;

    //set motors
    leftSlew(speed);
    rightSlew(speed);
  }
  return(0);
}

int turnTask(){
  int prevError;

  while(1){
    task::sleep(20);

    if(driveMode != 0)
      continue;

    int sp = turnTarget;

    double kp = turnKP;
    double kd = turnKD;

    int sv = (right1.rotation(rotationUnits::deg) - left1.rotation(rotationUnits::deg))/2;
    int error = sp-sv;
    int derivative = error - prevError;
    prevError = error;
    int speed = error*kp + derivative*kd;

    if(speed > maxSpeed)
      speed = maxSpeed;
    if(speed < -maxSpeed)
      speed = -maxSpeed;

    leftSlew(-speed);
    rightSlew(speed);
    }
  return(0);
}

void initDrive(){
  task drive_task(driveTask);
  task turn_task(turnTask);
}

void delay(int sleepTime){
  task::sleep(sleepTime);
}

/**************************************************/
//operator control
void tankOp(){
  driveMode = 2; //turns off autonomous tasks
  int lJoy = Controller1.Axis3.position();
  int rJoy = Controller1.Axis2.position();

  left(lJoy);
  right(rJoy);
}

void arcadeOp(){
  driveMode = 2; //turns off autonomous tasks
  int vJoy = Controller1.Axis3.position();
  int hJoy = Controller1.Axis4.position();

  left(vJoy + hJoy);
  right(vJoy - hJoy);
}
