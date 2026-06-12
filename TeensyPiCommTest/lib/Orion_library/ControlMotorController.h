#ifndef ControlMotorController_h
#define ControlMotorController_h

#include "controlMotor.h"

class motorController{

    private:
    controlMotor motorLeft;
    controlMotor motorBack;
    controlMotor motorRight;

    float currentAngle;
    float currentSpeed;

    public:
    motorController();
    void calibrate();
    void closedLoop();
    void positionMode();
    void velocityMode();
    void idle();
    void reset();

    float calculateMotorSpeed(controlMotor motor, float driveAngle, int targetVelocity);
    
    /**
     * Berechnet die Geschwindigkeiten für alle drei Motoren basierend auf der gewünschten Fahrtrichtung (angle) und Geschwindigkeit (speed) und setzt diese Geschwindigkeiten.
     * @param angle: Die gewünschte Fahrtrichtung in Grad (0-360).
     * @param speed: Die gewünschte Geschwindigkeit (0-100).
     */
    void drive(float angle, int speed);

    /**
     * Fügt eine Geschwindigkeitskomponente zum aktuellen Geschwindigkeitsvektor hinzu.
     * @param angle: Die Richtung der Geschwindigkeit in Grad (0-360).
     * @param speed: Die Geschwindigkeit (0-100).
     */
    void addToCurrentVelocityVector(float angle, float speed);

    /**
     * Fügt eine Drehgeschwindigkeit zum aktuellen Drehgeschwindigkeitsvektor hinzu.
     * @param turnspeed: Die gewünschte Drehgeschwindigkeit (0-100).
     */
    void addTurnSpeed(float turnspeed);

    /**
     * Dreht jedes Rad um die angegebene relative Position.
     * @param relativePosition: Die relative Position.
     */
    void addTurnPosition(float relativePosition);

    /**
     * Setzt die Drehgeschwindigkeit für alle drei Motoren.
     * @param speed: Die gewünschte Drehgeschwindigkeit (0-100).
     */
    void setTurnSpeed(float speed);

    /**
     * Gibt Informationen über alle Motoren in der Konsole aus.
     */
    void infoAll();

    /*
    * Eine Mehtode, die ein nach Zonen definiertes Bewegungsmuster implementiert, um den ball zwischen den Roboter
    * und das gegnerische Tor zu bewegen. Je nach der Position des Balls und des gegnerischen Tors werden
    * unterschiedliche Bewegungsstrategien angewendet.(siehe Orion onion 2.png)
    * @param ballDirection: Die Richtung zum Ball.
    * @param opponentGoalDirection: Die Richtung zum gegnerischen Tor.
    */
    void takeBall(float ballDirection, float opponentGoalDirection);

    controlMotor getLeftMotor();
    controlMotor getBackMotor();
    controlMotor getRightMotor();
};

#endif