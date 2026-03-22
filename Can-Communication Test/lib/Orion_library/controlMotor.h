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
    controlMotor(int nodeID);

    boolean inClosedLoop();
    void calibrate();
    void setIdle();
    void closedLoop();
    void reset();
    void positionMode();
    void velocityMode();

    void setVelocity(float speed);
    void addVelocity(float relativeSpeed);

    void setPosition(float position);
    void addPosition(float relativePosition);

    float getID();
    float getTemp();
    float getState();
    float getMode();
    float getPositionEstimate();
    float getVelocityEstimate();
    float getIqEstimate();
    float getIqSetpoint();
    int   getAngle();

    void info();
};

#endif