#include "Arduino.h"
#include <FlexCAN_T4.h>
#include <tinymovr.hpp>

FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

void send_cb(uint32_t arbitration_id, uint8_t *data, uint8_t data_size, bool rtr)
{
  CAN_message_t tx;
  tx.id = arbitration_id;
  tx.len = data_size;

  tx.flags.extended = 1;

  if (rtr) {
    tx.flags.remote = 1;
    tx.len = 0;
  } else {
    tx.flags.remote = 0;
    tx.len = data_size;
    for (uint8_t i=0; i<data_size && i<8; i++) tx.buf[i] = data[i];
  }

  Serial.print("CAN TX id=0x"); Serial.print(arbitration_id, HEX);
  Serial.print(rtr ? " RTR" : " DATA");
  Serial.print(" len="); Serial.println(rtr ? 0 : data_size);

  Can1.write(tx);
}

bool recv_cb(uint32_t *arbitration_id, uint8_t *data, uint8_t *data_size)
{
  CAN_message_t rx;
  if (Can1.read(rx)) {
    Serial.print("CAN RX ID=0x"); Serial.print(rx.id, HEX);
    Serial.print(rx.flags.extended ? " EXT " : " STD ");
    Serial.print("LEN="); Serial.print(rx.len);
    Serial.print(" DATA=");
    for (int i = 0; i < rx.len; i++) {
      if (rx.buf[i] < 16) Serial.print('0');
      Serial.print(rx.buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    *arbitration_id = rx.id;
    *data_size = rx.len;

    for (uint8_t i = 0; i < rx.len && i < 8; i++) {
      data[i] = rx.buf[i];
    }
    return true;
  }
  return false;
}

void delay_us_cb(uint32_t us)
{
  delayMicroseconds(us);
}

Tinymovr tinymovr(1, &send_cb, &recv_cb, &delay_us_cb, 100);

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 2000) {}

  Can1.begin();
  Can1.setBaudRate(1000000);

  Can1.enableFIFO();

  Can1.setFIFOFilter(ACCEPT_ALL);

  if (tinymovr.get_protocol_hash() != avlos_proto_hash)
  {
    Serial.println("Wrong device spec!");
    Serial.print("Device hash: 0x");
    Serial.println(dev_hash, HEX);
    while (1) {}
  }

  Serial.println("Teensy + FlexCAN_T4 ready.");
}

void loop()
{
  if (Serial.available() > 0) {
    uint8_t receivedChar = Serial.read();

    if (receivedChar == 'Q') {
      Serial.println("Received Calibration command");
      tinymovr.controller.set_state(1);
    }
    else if (receivedChar == 'A') {
      Serial.println("Received Closed Loop command");
      tinymovr.controller.set_state(2);
      tinymovr.controller.set_mode(2);
    }
    else if (receivedChar == 'Z') {
      Serial.println("Received Idle command");
      tinymovr.controller.set_state(0);
    }
    else if (receivedChar == 'R') {
      Serial.println("Received reset command");
      tinymovr.reset();
    }
    else if (receivedChar == '<') {
      Serial.println("Received L turn command");
      float pos_estimate = tinymovr.sensors.user_frame.get_position_estimate();
      Serial.println(pos_estimate);
      tinymovr.controller.position.set_setpoint(pos_estimate - 8192.0f);
    }
    else if (receivedChar == '>') {
      Serial.println("Received R turn command");
      float pos_estimate = tinymovr.sensors.user_frame.get_position_estimate();
      Serial.println(pos_estimate);
      tinymovr.controller.position.set_setpoint(pos_estimate + 8192.0f);
    }
    else if (receivedChar == 'I') {
      Serial.print("Device ID: ");
      Serial.print(tinymovr.comms.can.get_id());
      Serial.print(", Temp:");
      Serial.print(tinymovr.get_temp());
      Serial.print(", State:");
      Serial.print(tinymovr.controller.get_state());
      Serial.print(", Mode:");
      Serial.print(tinymovr.controller.get_mode());
      Serial.print("\n");

      Serial.print("Position estimate: ");
      Serial.print(tinymovr.sensors.user_frame.get_position_estimate());
      Serial.print(", Velocity estimate: ");
      Serial.print(tinymovr.sensors.user_frame.get_velocity_estimate());
      Serial.print("\n");

      Serial.print("Iq estimate: ");
      Serial.print(tinymovr.controller.current.get_Iq_estimate());
      Serial.print(", Iq setpoint: ");
      Serial.print(tinymovr.controller.current.get_Iq_setpoint());
      Serial.print("\n---\n");
    }
  }

  delay(50);
}