#include "ControlMotorController.h"

motorController::motorController()   // constructor
   :motorLeft(1),
    motorBack(2),
    motorRight(3)
    {}

    float currentAngle = 0;
    float currentSpeed = 0;

    void motorController::calibrate() {
        motorLeft.calibrate();
        motorBack.calibrate();
        motorRight.calibrate();
    }

    void motorController::closedLoop() {
        motorLeft.closedLoop();
        motorBack.closedLoop();
        motorRight.closedLoop();
    }

    void motorController::positionMode() {
        motorLeft.positionMode();
        motorBack.positionMode();
        motorRight.positionMode();
    }

    void motorController::velocityMode() {
        motorLeft.velocityMode();
        motorBack.velocityMode();
        motorRight.velocityMode();
    }

    void motorController::idle() {
        motorLeft.setIdle();
        motorBack.setIdle();
        motorRight.setIdle();
    }

    void motorController::reset() {
        motorLeft.reset();
        motorBack.reset();
        motorRight.reset();
    }

    float motorController::calculateMotorSpeed(controlMotor motor, float driveAngle, int targetVelocity)
    {
        // get the angle from the weel to driveAngle and put into radians
        double radians = (driveAngle - motor.getAngle()) * 3.14159 / 180;

        // calculates the speed the motor has to turn
        float motorSpeed = (float) cos(radians) * targetVelocity;
        return motorSpeed;
    }

    // calculates and sets the speed for all motors to drive with given speed into given direction
    void motorController::drive(float angle, int speed) {
        float currentAngle = angle;
        float currentSpeed = speed;
        motorLeft.setVelocity(calculateMotorSpeed(motorLeft, angle, speed));
        motorBack.setVelocity(calculateMotorSpeed(motorBack, angle, speed));
        motorRight.setVelocity(calculateMotorSpeed(motorRight, angle, speed));
    }

    void motorController::addToCurrentVelocityVector(float angle, float speed) {
        float currentAngle = angle;
        float currentSpeed = speed;
        motorLeft.addVelocity(calculateMotorSpeed(motorLeft, angle, speed));
        motorBack.addVelocity(calculateMotorSpeed(motorBack, angle, speed));
        motorRight.addVelocity(calculateMotorSpeed(motorRight, angle, speed));
    }

    // adds a speed to all motors to achieve a turning motion
    void motorController::addTurnSpeed(float turnspeed) {
        motorLeft.addVelocity(turnspeed);
        motorBack.addVelocity(turnspeed);
        motorRight.addVelocity(turnspeed);
    }

    void motorController::addPosition(float relativePosition) {
        motorLeft.addPosition(relativePosition);
        motorBack.addPosition(relativePosition);
        motorRight.addPosition(relativePosition);
    }

    void motorController::setTurnSpeed(float speed) {
        motorLeft.setVelocity(speed);
        motorBack.setVelocity(speed);
        motorRight.setVelocity(speed);
    }

    // prints the info of all Motors into the console
    void motorController::infoAll() {
        motorLeft.info();
        Serial.println("");
        motorBack.info();
        Serial.println("");
        motorRight.info();
    }

    void motorController::takeBall(float ballDirection, float opponentGoalDirection)
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

    // getter methods
    controlMotor motorController::getLeftMotor()     {   return motorLeft;   }
    controlMotor motorController::getBackMotor()     {   return motorBack;   }
    controlMotor motorController::getRightMotor()    {   return motorRight;  }
    float getCurrentAngle() { return currentAngle; }
    float getCurrentSpeed() { return currentSpeed; }