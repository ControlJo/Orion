#include "controlMotor.h"

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

// ---------------------- Constructor ------------------------
controlMotor::controlMotor(int nodeID)
: tinymovr(nodeID, &send_cb, &recv_cb, &delay_us_cb, 100)
{
  switch (nodeID){
    case 1: angleToZeroDegrees = -30; break;
    case 2: angleToZeroDegrees = 90;  break;
    case 3: angleToZeroDegrees = 30;  break;
    default:
      Serial.println("Error in constructor of controlMotor: Incorrect Node ID");
      angleToZeroDegrees = 0;
      break;
  }
}

// ---------------------- Methods ------------------------
boolean controlMotor::inClosedLoop() { return tinymovr.controller.get_state() == 2; }

void controlMotor::calibrate()    { tinymovr.controller.set_state(1); }
void controlMotor::setIdle()      { tinymovr.controller.set_state(0); }
void controlMotor::closedLoop()   { tinymovr.controller.set_state(2); }
void controlMotor::reset()        { tinymovr.reset(); }
void controlMotor::positionMode() { tinymovr.controller.set_mode(2); }
void controlMotor::velocityMode() { tinymovr.controller.set_mode(2); }

void controlMotor::setVelocity(float speed) {
  tinymovr.controller.velocity.set_setpoint(speed * 10000);
}

void controlMotor::addVelocity(float relativeSpeed) {
  velocitySetpoint += relativeSpeed;
  tinymovr.controller.velocity.set_setpoint(velocitySetpoint);
}

void controlMotor::setPosition(float position) {
  tinymovr.controller.position.set_setpoint(position);
}

void controlMotor::addPosition(float relativePosition) {
  positionSetpoint += relativePosition;
  tinymovr.controller.position.set_setpoint(positionSetpoint);
}

// ---------------------- Getter ------------------------
float controlMotor::getID()               { return tinymovr.comms.can.get_id(); }
float controlMotor::getTemp()             { return tinymovr.get_temp(); }
float controlMotor::getState()            { return tinymovr.controller.get_state(); }
float controlMotor::getMode()             { return tinymovr.controller.get_mode(); }
float controlMotor::getPositionEstimate() { return tinymovr.sensors.user_frame.get_position_estimate(); }
float controlMotor::getVelocityEstimate() { return tinymovr.sensors.user_frame.get_velocity_estimate(); }
float controlMotor::getIqEstimate()       { return tinymovr.controller.current.get_Iq_estimate(); }
float controlMotor::getIqSetpoint()       { return tinymovr.controller.current.get_Iq_setpoint(); }
int   controlMotor::getAngle()            { return angleToZeroDegrees; }

void controlMotor::info() {
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