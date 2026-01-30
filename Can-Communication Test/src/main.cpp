#include "controlMotor.cpp"
#include "Arduino.h"
#include "FlexCAN_T4.h"
#include "tinymovr.hpp"

// ---------------------- Motor objects --------------------------
controlMotor motorVorneLinks(1);
controlMotor motorVorneRechts(3);
controlMotor motorHinten(2);

// ---------------------- Arduino setup -----------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 2000) {}

  // Start CAN1 @ 1 Mbps
  Can1.begin();
  Can1.setBaudRate(1000000);
  Can1.enableFIFO();
  Can1.setFIFOFilter(ACCEPT_ALL);

  Serial.println("Teensy + FlexCAN_T4 ready.");
}

// ---------------------- Arduino loop -----------------------------
void loop()
{
  while (Serial.available() > 0) {
    char receivedChar = Serial.read();

    // ---------------- Normale Buchstabenbefehle ----------------
    if (receivedChar == 'Q') {
      Serial.println("Received Calibration command");
      motorVorneLinks.calibrate();
      motorVorneRechts.calibrate();
      motorHinten.calibrate();
    }
    else if (receivedChar == 'A') {
      Serial.println("Received Closed Loop command");
      motorVorneLinks.closedLoop();
      motorVorneRechts.closedLoop();
      motorHinten.closedLoop();
      motorVorneLinks.positionMode(); // Position Mode
      motorVorneRechts.positionMode();
      motorHinten.positionMode();
    }
    else if (receivedChar == 'Z') {
      Serial.println("Received Idle command");
      motorVorneLinks.idle();
      motorVorneRechts.idle();
      motorHinten.idle();
    }
    else if (receivedChar == 'R') {
      Serial.println("Received reset command");
      motorVorneLinks.reset();
      motorVorneRechts.reset();
      motorHinten.reset();
    }
    else if (receivedChar == '<') {
      Serial.println("Received L turn command");
      motorVorneLinks.addPosition(-8192);
      motorVorneRechts.addPosition(-8192);
      motorHinten.addPosition(-8192);
    }
    else if (receivedChar == '>') {
      Serial.println("Received R turn command");
      motorVorneLinks.addPosition(8192);
      motorVorneRechts.addPosition(8192);
      motorHinten.addPosition(8192);
    }
    else if (receivedChar == 'I') {
      motorVorneLinks.info();
      motorVorneRechts.info();
      motorHinten.info();
    }
    else if (receivedChar == 'V') {
      Serial.println("Switching to Velocity Mode");
      motorVorneLinks.closedLoop(); // Closed Loop
      motorVorneLinks.velocityMode();  // Velocity Mode
      motorVorneLinks.setVelocity(0);
      Serial.println("Velocity set to 0");
    }
    else if( receivedChar == 'p') {
      motorVorneLinks.addVelocity(1);
    }
    else if( receivedChar == 'o') {
      motorVorneLinks.addVelocity(-1);
    }
    // ----------------- driving control ---------------------
    else if( receivedChar == 'w') {
      Serial.print("Received forwards drive command");
      motorVorneLinks.addPosition(-200);
      motorVorneRechts.addPosition(200);
    }
    else if( receivedChar == 's') {
      Serial.print("Received backwards drive command");
      motorVorneLinks.addPosition(200);
      motorVorneRechts.addPosition(-200);
    }
    else if( receivedChar == 'a') {
      Serial.print("Received left drive command");
      motorVorneLinks.addPosition(200);
      motorVorneRechts.addPosition(200);
      motorHinten.addPosition(-200);
    }
    else if( receivedChar == 'd') {
      Serial.print("Received right drive command");
      motorVorneLinks.addPosition(-200);
      motorVorneRechts.addPosition(-200);
      motorHinten.addPosition(200);
    }
    else if( receivedChar == 'q') {
      Serial.print("Received left turn command");
      motorVorneLinks.addPosition(200);
      motorVorneRechts.addPosition(200);
      motorHinten.addPosition(200);
    }
    else if( receivedChar == 'e') {
      Serial.print("Received right turn command");
      motorVorneLinks.addPosition(-200);
      motorVorneRechts.addPosition(-200);
      motorHinten.addPosition(-200);
    }
  }
  delay(50);
}