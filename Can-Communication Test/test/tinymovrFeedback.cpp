#include "Arduino.h"
#include <FlexCAN_T4.h>  // Speziell für Teensy 4.1
#include <tinymovr.hpp>

// ---------------------------------------------------------------
// HARDWARE SETUP TEENSY 4.1 (Pins 22 TX, 23 RX)
// ---------------------------------------------------------------
FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> Can1;

void send_cb(uint32_t arbitration_id, uint8_t *data, uint8_t data_size, bool rtr)
{
  CAN_message_t msg;
  msg.id = arbitration_id;
  msg.len = data_size;
  msg.flags.remote = rtr;
  memcpy(msg.buf, data, data_size);
  Can1.write(msg);
}

bool recv_cb(uint32_t *arbitration_id, uint8_t *data, uint8_t *data_size)
{
  CAN_message_t msg;
  // Wir lesen den Puffer, bis wir eine Nachricht finden, die KEIN Heartbeat ist
  while (Can1.read(msg)) {
    // Heartbeat Filter: Tinymovr sendet auf Endpoint 0 (letzte 5 bits der ID sind 0)
    // Wenn (ID & 0x1F) == 0, ist es ein Heartbeat -> ignorieren und weiterlesen
    if ((msg.id & 0x1F) == 0) {
      continue; 
    }
    
    // Gültige Daten-Antwort gefunden:
    *data_size = msg.len;
    *arbitration_id = msg.id;
    memcpy(data, msg.buf, msg.len);
    return true;
  }
  return false;
}

void delay_us_cb(uint32_t us)
{
  delayMicroseconds(us);
}

// ---------------------------------------------------------------
// TINYMOVR INSTANZEN & WRAPPER KLASSE
// ---------------------------------------------------------------

// Instanzen für die 3 Motoren
Tinymovr tm1(1, &send_cb, &recv_cb, &delay_us_cb, 100);
Tinymovr tm2(2, &send_cb, &recv_cb, &delay_us_cb, 100);
Tinymovr tm3(3, &send_cb, &recv_cb, &delay_us_cb, 100);

class controlMotor {
  public:
    Tinymovr &tm;
    controlMotor(Tinymovr &_tm) : tm(_tm) {}

    // Hier ist die fehlende idle Methode
    void idle() {
      tm.controller.set_state(0);
    }

    void set_state(uint8_t state) { tm.controller.set_state(state); }
    void set_mode(uint8_t mode) { tm.controller.set_mode(mode); }
    void reset() { tm.reset(); }
    float get_position() { return tm.sensors.user_frame.get_position_estimate(); }
    void set_position(float p) { tm.controller.position.set_setpoint(p); }
};

controlMotor motorVorneLinks(tm1);
controlMotor motorVorneRechts(tm2);
controlMotor motorHinten(tm3);

// ---------------------------------------------------------------
// SETUP
// ---------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  
  // Teensy 4.1 CAN Initialisierung
  Can1.begin();
  Can1.setBaudRate(1000000); 

  Serial.println("Teensy 4.1 CAN gestartet.");

  // Prüfung (Das "Rot" im Editor bei avlos_proto_hash verschwindet nach dem ersten Build)
  if (tm1.get_protocol_hash() != avlos_proto_hash)
  {
    Serial.println("Protocol Hash Mismatch! Prüfe Library Version.");
  }
}

// ---------------------------------------------------------------
// LOOP
// ---------------------------------------------------------------

void loop() 
{
  if (Serial.available() > 0) {
    uint8_t receivedChar = Serial.read();
    
    if (receivedChar == 'Q') {
      Serial.println("Calibration...");
      motorVorneLinks.set_state(1);
    }
    else if (receivedChar == 'A') {
      Serial.println("Closed Loop...");
      motorVorneLinks.set_state(2);
      motorVorneLinks.set_mode(2);
    }
    else if (receivedChar == 'Z') {
      Serial.println("Idle...");
      motorVorneLinks.idle();
      motorVorneRechts.idle();
      motorHinten.idle();
    }
    else if (receivedChar == 'R') {
      motorVorneLinks.reset();
    }
    else if (receivedChar == 'I') {
      // Board Information mit Rückmeldung (get_temp, get_vbus etc.)
      Serial.println("--- Tinymovr Status ---");
      Serial.print("M1 Temp: "); Serial.print(tm1.get_temp());
      Serial.print(" | VBus: "); Serial.print(tm1.get_vbus());
      Serial.print(" | Pos: "); Serial.println(motorVorneLinks.get_position());
      Serial.println("---");
    }
  }
  delay(10);
}