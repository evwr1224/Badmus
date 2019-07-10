#include "vex.h"
#include "drive.h"
#include "systems.h"

brain Brain;
competition Competition;
controller Controller1;

/*****************************************************/
//autons
void redAuton(){
  //insert auton code here
}


void blueAuton(){
  //insert auton code here
}


/*****************************************************/
//initialization (runs when the robot turns on)
void pre_auton( void ) {
  initDrive();
}


/*****************************************************/
//autonomous selection (runs when auton starts)
void autonomous( void ) {

  if(Brain.ThreeWirePort.A.value()){
    redAuton();
  }else if (Brain.ThreeWirePort.B.value()){
    blueAuton();
  }
}


/*****************************************************/
//driver control
void usercontrol( void ) {
  while (1) {
    tankOp(); // tank drive function

    //intake driver control
    if(Controller1.ButtonR2.pressing()){
      intake(100);
    }else if(Controller1.ButtonL2.pressing()){
      intake(-100);
    }else{
      intake(0);
    }

    //lift driver control
    if(Controller1.ButtonL1.pressing()){
      lift(100);
    }else if(Controller1.ButtonR1.pressing()){
      lift(-100);
    }else{
      lift(0);
    }

    //autonomous button (starts auton from driver control for testing purposes)
    if(Controller1.ButtonLeft.pressing() && !Competition.isFieldControl()){
      autonomous();
    }

    delay(20);
  }
}


/*****************************************************/
//main loop - modify at your own risk! -
int main() {
    //Set up callbacks for autonomous and driver control periods.
    Competition.autonomous( autonomous );
    Competition.drivercontrol( usercontrol );
    
    //Run the pre-autonomous function. 
    pre_auton();
       
    //Prevent main from exiting with an infinite loop.                        
    while(1) {
      task::sleep(100);
    }    
       
}
