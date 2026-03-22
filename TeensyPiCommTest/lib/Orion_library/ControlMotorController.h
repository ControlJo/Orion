#ifndef ControlMotorController_h
#define ControlMotorController_h

#include "controlMotor.h"

class motorController{

    private:
    controlMotor motorLeft;
    controlMotor motorBack;
    controlMotor motorRight;

    public:
    motorController();
    void calibrate();
    void closedLoop();
    void positionMode();
    void velocityMode();
    void idle();
    void reset();

    float calculateMotorSpeed(controlMotor motor, float driveAngle, int targetVelocity);
    
    void drive(float angle, int speed);

    void addToCurrentVelocityVector(float angle, float speed);
    void addTurnSpeed(float turnspeed);
    void addPosition(float relativePosition);
    void setVelocity(float speed);

    void infoAll();
    void takeBall(float ballDirection, float opponentGoalDirection);

    controlMotor getLeftMotor();
    controlMotor getBackMotor();
    controlMotor getRightMotor();
};

#endif