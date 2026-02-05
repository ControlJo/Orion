#include "controlMotor.cpp"


class MotorController{
    // initialization list
    // -> makes clear the following objects have to be initialized before calling the constructor
    private:
    controlMotor motorLeft;
    controlMotor motorBack;
    controlMotor motorRight;

    MotorController()   // constructor
       :motorLeft(1),
        motorBack(2),
        motorRight(3)
    {}

    // calls the calibrate method for all motors
    void CalibrateAll() {
        motorLeft.calibrate();
        motorBack.calibrate();
        motorRight.calibrate();
    }

    float calculateMotorSpeed(controlMotor motor, float driveAngle, int targetVelocity)
    {
        // get the angle from the weel to driveAngle and put into radians
        double radians = (driveAngle - motor.getAngle()) * 3.14159 / 180;

        // calculates the speed the motor has to turn
        float motorSpeed = (float) cos(radians) * targetVelocity;
        return motorSpeed;
    }

    // calculates and sets the speed for all motors to drive with given speed into given direction
    void drive(float angle, float speed) {
        motorLeft.setVelocity(calculateMotorSpeed(motorLeft, angle, speed));
        motorBack.setVelocity(calculateMotorSpeed(motorBack, angle, speed));
        motorRight.setVelocity(calculateMotorSpeed(motorRight, angle, speed));
    }

    // adds a speed to all motors to achieve a turning motion
    void addTurnSpeed(float turnspeed) {
        motorLeft.addVelocity(turnspeed);
        motorBack.addVelocity(turnspeed);
        motorRight.addVelocity(turnspeed);
    }

    // prints the info of all Motors into the console
    void infoAll() {
        motorLeft.info();
        Serial.println("");
        motorBack.info();
        Serial.println("");
        motorRight.info();
    }

    void takeBall(float ballDirection, float opponentGoalDirection)
    {
        float driveAngle = 0;

        // evaluate the best approach to the ball
        if      (320 < ballDirection && ballDirection < 40) {driveAngle = ballDirection;}
        else if (40  < ballDirection && ballDirection < 80) {driveAngle = 90;}
        else if (80  < ballDirection && ballDirection < 100){driveAngle = 135;}
        else if (100 < ballDirection && ballDirection < 150){driveAngle = 180;}
        else if (150 < ballDirection && ballDirection < 210)   // ball behind robot
        { 
          if(0   <= opponentGoalDirection && opponentGoalDirection <= 180){driveAngle = 135;}
          if(180 <  opponentGoalDirection && opponentGoalDirection <= 360){driveAngle = 225;}
        }
        else if (210 < ballDirection && ballDirection < 260){driveAngle = 180;}
        else if (260 < ballDirection && ballDirection < 280){driveAngle = 225;}
        else if (280 < ballDirection && ballDirection < 320){driveAngle = 270;}

        drive(driveAngle, 100);
    }

    // returns the object of one motor to make indivitual actions possible
    controlMotor getLeftMotor()     {   return motorLeft;   }
    controlMotor getBackMotor()     {   return motorLeft;   }
    controlMotor getRightMotor()    {   return motorLeft;   }

};