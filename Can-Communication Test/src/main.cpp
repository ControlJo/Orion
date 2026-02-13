#include "Arduino.h"
#include "FlexCAN_T4.h"
#include "tinymovr.hpp"
#include "controlMotorController.cpp"

// ---------------------- Motor controller object --------------------------

motorController controller;

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
      controller.calibrateAll();
    }
    else if (receivedChar == 'A') {
      Serial.println("Received Closed Loop command");
      controller.closedLoop();
      controller.positionMode();
    }
    else if (receivedChar == 'Z') {
      Serial.println("Received Idle command");
      controller.idle();
    }
    else if (receivedChar == 'R') {
      Serial.println("Received reset command");
      controller.reset();
    }
    else if (receivedChar == '<') {
      Serial.println("Received L turn command");
      controller.addPosiition(200);
    }
    else if (receivedChar == '>') {
      Serial.println("Received R turn command");
      controller.addPosiition(-200);
    }
    else if (receivedChar == 'I') {
      controller.infoAll();
    }
    else if (receivedChar == 'V') {
      Serial.println("Switching to Velocity Mode");
      controller.closedLoop(); // Closed Loop
      controller.velocityMode();  // Velocity Mode
      controller.setVelocity(0);
      Serial.println("Velocity set to 0");
    }
    else if( receivedChar == 'p') {
      controller.getLeftMotor().addVelocity(1);
    }
    else if( receivedChar == 'o') {
      controller.getLeftMotor().addVelocity(-1);
    }
    // ----------------- driving control ---------------------
    else if( receivedChar == 'w') {
      Serial.print("Received forwards drive command");
      controller.drive(0, 100);
    }
    else if( receivedChar == 's') {
      Serial.print("Received backwards drive command");
      controller.drive(180, 100);
    }
    else if( receivedChar == 'a') {
      Serial.print("Received left drive command");
      controller.drive(90, 100);
    }
    else if( receivedChar == 'd') {
      Serial.print("Received right drive command");
      controller.drive(270, 100);
    }
    else if( receivedChar == 'q') {
      Serial.print("Received left turn command");
      controller.addPosiition(200);
    }
    else if( receivedChar == 'e') {
      Serial.print("Received right turn command");
      controller.addPosiition(-200);
    }
  }
  delay(50);
}