#include "ControlMotorController.h"

// ---------------------- Motor controller object --------------------------

motorController controller;

// ------------------------ pi comm ------------------------------------

int ballrichtung;
int gelbtor;
int blautor;

// ---------------------- Arduino setup -----------------------------
void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 2000) {}

  Serial1.begin(9600);

  // Start CAN1 @ 1 Mbps
  Can1.begin();
  Can1.setBaudRate(1000000);
  Can1.enableFIFO();
  Can1.setFIFOFilter(ACCEPT_ALL);

  Serial.println("Teensy + FlexCAN_T4 ready.");
}

void doPiCommunication()
{
  String receivedData;
  if(Serial1.available())
  {
    receivedData = Serial1.readStringUntil('\n');
    Serial.print("Empfangen: ");
    Serial.println(receivedData);
  }
  sscanf(receivedData.c_str(), "%d %d %d", &ballrichtung, &gelbtor, &blautor);
}

// ---------------------- Arduino loop -----------------------------
void loop()
{

  Serial.println("test");
  doPiCommunication();

  while (Serial.available() > 0) {
    char receivedChar = Serial.read();

    // ---------------- Normale Buchstabenbefehle ----------------
    if (receivedChar == 'Q') {
      Serial.println("Received Calibration command");
      controller.calibrate();
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
      controller.addPosition(200);
    }
    else if (receivedChar == '>') {
      Serial.println("Received R turn command");
      controller.addPosition(-200);
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
      controller.addPosition(200);
    }
    else if( receivedChar == 'e') {
      Serial.print("Received right turn command");
      controller.addPosition(-200);
    }
  }
  delay(50);
}