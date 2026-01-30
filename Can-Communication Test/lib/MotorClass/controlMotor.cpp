#include "Arduino.h"
#include "FlexCAN_T4.h"
#include "tinymovr.hpp"

// ---------------------- FlexCAN_T4 instance ----------------------
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

// ---------------------- REQUIRED CALLBACKS ------------------------
void send_cb(uint32_t arbitration_id, uint8_t *data, uint8_t data_size, bool rtr)
{
  CAN_message_t tx;
  tx.id = arbitration_id;
  tx.len = data_size;
  tx.flags.extended = 1;
  tx.flags.remote   = rtr;
  for (uint8_t i = 0; i < data_size && i < 8; i++) tx.buf[i] = data[i];
  Can1.write(tx);
}

bool recv_cb(uint32_t *arbitration_id, uint8_t *data, uint8_t *data_size)
{
  CAN_message_t rx;
  if (Can1.read(rx)) {
    *arbitration_id = rx.id;
    *data_size = rx.len;
    for (uint8_t i = 0; i < rx.len && i < 8; i++) data[i] = rx.buf[i];
    return true;
  }
  return false;
}

void delay_us_cb(uint32_t us) { delayMicroseconds(us); }

// ---------------------- eigenes -------------------------

class controlMotor{

  //-------------------------- Variables ----------------
  float velocitySetpoint = 0;
  float positionSetpoint = 0;
    
  private:
  Tinymovr tinymovr;

  public:
  controlMotor(int nodeID)
  :tinymovr(nodeID, &send_cb, &recv_cb, &delay_us_cb, 100)
  {}

  boolean inClosedLoop() { return tinymovr.controller.get_state() == 2; }

  void calibrate()  { tinymovr.controller.set_state(1); }

  void idle()       { tinymovr.controller.set_state(0); }

  void closedLoop() { tinymovr.controller.set_state(2); }

  void reset()      { tinymovr.reset(); }

  void positionMode() { tinymovr.controller.set_mode(2); }

  void velocityMode() { tinymovr.controller.set_mode(2); }

  // from 0 to 100 %, tinymovr max is 1 000 000 so multiply with 10 000 ( 100 * 10 000 = 1 000 000)
  void setVelocity(float speed) {
    tinymovr.controller.velocity.set_setpoint(speed * 10000);
  }

  // method to in-/decrease the velocity based on the set value
  void addVelocity(float relativeSpeed) {
    tinymovr.controller.velocity.set_setpoint(velocitySetpoint + relativeSpeed);
    velocitySetpoint += relativeSpeed;
  }
  
  void setPosition(float position) {
    tinymovr.controller.position.set_setpoint(position);
  }

  // method to in-/decrease the position based on the set value
  void addPosition(float relativePosition) {
    tinymovr.controller.position.set_setpoint(positionSetpoint + relativePosition);
    positionSetpoint += relativePosition;
  }
  
  // ------------------- getter methods -----------------------------

  float getID()               { return tinymovr.comms.can.get_id();                         }
  float getTemp()             { return tinymovr.get_temp();                                 }
  float getState()            { return tinymovr.controller.get_state();                     }
  float getMode()             { return tinymovr.controller.get_mode();                      }
  float getPositionEstimate() { return tinymovr.sensors.user_frame.get_position_estimate(); }
  float getVelocityEstimate() { return tinymovr.sensors.user_frame.get_velocity_estimate(); }
  float getIqEstimate()       { return tinymovr.controller.current.get_Iq_estimate();       }
  float getIqSetpoint()       { return tinymovr.controller.current.get_Iq_setpoint();       }

  void info() {
    Serial.print("Device ID: ");            Serial.print(getID());
    Serial.print(", Temp: ");               Serial.print(getTemp());
    Serial.print(", State: ");              Serial.print(getState());
    Serial.print(", Mode: ");               Serial.print(getMode());
    Serial.print("\n");

    Serial.print("Position estimate: ");    Serial.print(getPositionEstimate());
    Serial.print(", Velocity estimate: ");  Serial.print(getVelocityEstimate());
    Serial.print("\n");

    Serial.print("Iq estimate: ");          Serial.print(getIqEstimate());
    Serial.print(", Iq setpoint: ");        Serial.print(getIqSetpoint());
    Serial.print("\n---\n");
  }
};