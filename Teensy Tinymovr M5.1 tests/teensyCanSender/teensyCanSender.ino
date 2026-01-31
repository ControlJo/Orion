#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

void setup() {
  Serial.begin(115200);
  Can1.begin();
  Can1.setBaudRate(1000000);
  Serial.println("Sender ready");
}

void loop() {
  CAN_message_t msg;
  msg.id = 0x123;
  msg.flags.extended = 1;
  msg.len = 8;
  msg.buf[0] = 0xDE;
  msg.buf[1] = 0xAD;
  msg.buf[2] = 0xBE;
  msg.buf[3] = 0xEF;
  msg.buf[4] = 0x01;
  msg.buf[5] = 0x02;
  msg.buf[6] = 0x03;
  msg.buf[7] = 0x04;

  Can1.write(msg);
  delay(500);
}