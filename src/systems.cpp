#include "vex.h"

/*************************************************************/
//intake 
motor intake_motor(PORT22, gearSetting::ratio18_1, false);
void intake(int speed){
  intake_motor.spin(directionType::fwd, speed, velocityUnits::pct);
}

/*************************************************************/
//lift
motor lift_motor1(PORT1, gearSetting::ratio18_1, false);
motor lift_motor2(PORT2, gearSetting::ratio18_1, true);
void lift(int speed){
  lift_motor1.spin(directionType::fwd, speed, velocityUnits::pct);
  lift_motor2.spin(directionType::fwd, speed, velocityUnits::pct);
}

