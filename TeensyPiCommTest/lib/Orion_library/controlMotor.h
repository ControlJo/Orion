#ifndef controlMotor_h
#define controlMotor_h

#include <Arduino.h>
#include "FlexCAN_T4.h"
#include "tinymovr.hpp"

extern FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

void send_cb(uint32_t arbitration_id, uint8_t *data, uint8_t data_size, bool rtr);
bool recv_cb(uint32_t *arbitration_id, uint8_t *data, uint8_t *data_size);
void delay_us_cb(uint32_t us);

class controlMotor {
private:
    float velocitySetpoint = 0;
    float positionSetpoint = 0;
    int angleToZeroDegrees;

    Tinymovr tinymovr;
    
public:
    /**
     * Konstruktor für die controlMotor Klasse.
     * @param nodeID: Die Node-ID des Tinymovr.
     */
    controlMotor(int nodeID);

    void startCan();

    boolean inClosedLoop();

    /**
     * Sendet den Kalibrierungsbefehl an den Tinymovr.
     */
    void calibrate();

    /**
     * Setzt den Motor in den Idle-Modus.
     */
    void setIdle();

    /**
     * Schaltet den Motor in den Closed-Loop-Modus.
     */
    void closedLoop();

    /**
     * Setzt den Motor zurück.
     */
    void reset();

    /**
     * Schaltet den Motor in den Positionierungsmodus.
     */
    void positionMode();

    /**
     * Schaltet den Motor in den Geschwindigkeitsmodus.
     */
    void velocityMode();
    
    /**
     * Setzt die Geschwindigkeit des Motors.
     * @param speed: Die gewünschte Geschwindigkeit (0-100).
     * Die Methode begrenzt die Geschwindigkeit auf den Bereich von -100 bis 100.
     */
    void setVelocity(float speed);

    /**
     * Fügt eine relative Geschwindigkeit zum aktuellen Geschwindigkeitssetpoint hinzu.
     * @param relativeSpeed: Die relative Geschwindigkeit (-100-100).
     * Die Methode begrenzt die Geschwindigkeit auf den Bereich von -100 bis 100.
     */
    void addVelocity(float relativeSpeed);

    /**
     * Setzt die Position des Motors.
     * @param position: Die gewünschte Position.
     * Die Methode begrenzt die Position auf den Bereich von -100 bis 100.
     */
    void setPosition(float position);

    /**
     * Fügt eine relative Position zum aktuellen Positionssetpoint hinzu.
     * @param relativePosition: Die relative Position.
     */
    void addPosition(float relativePosition);

    float getID();
    float getTemp();
    float getState();
    float getMode();
    float getPositionEstimate();
    float getPositionSetpoint();
    float getVelocityEstimate();
    float getVelocitySetpoint();
    float getIqEstimate();
    float getIqSetpoint();
    int   getAngle();

    void info();
};

#endif