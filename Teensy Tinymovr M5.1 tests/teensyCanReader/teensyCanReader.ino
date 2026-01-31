#include <FlexCAN_T4.h>
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

void setup() {
  Serial.begin(115200);
  Can1.begin();
  Can1.setBaudRate(1000000);
  Can1.enableFIFO();
  Can1.setFIFOFilter(ACCEPT_ALL);
  Serial.println("Sniffer ready");
}

void loop() {
  CAN_message_t rx;
  if (Can1.read(rx)) {
    Serial.print("ID=0x");
    Serial.print(rx.id, HEX);
    Serial.print(rx.flags.extended ? " EXT " : " STD ");
    Serial.print("LEN=");
    Serial.print(rx.len);
    Serial.print(" DATA=");
    for (int i = 0; i < rx.len; i++) {
      if (rx.buf[i] < 16) Serial.print('0');
      Serial.print(rx.buf[i], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }
}