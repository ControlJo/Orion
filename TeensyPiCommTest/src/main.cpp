#include "ControlMotorController.h"

int ballrichtung = 0;
int gelbtor = 0;
int blautor = 0;

motorController controller;

// Liest die Daten vom Pi und speichert sie in ballrichtung, gelbtor und blautor
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //Serial1.begin(9600);
  //Serial2.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  doPiCommunication();

  Serial.println("[INFO]: Ballrichtung: " + String(ballrichtung));
  Serial.println("[INFO]: Gelbtor: " + String(gelbtor));
  Serial.println("[INFO]: Blautor: " + String(blautor));
  
  controller.drive(ballrichtung, 10);
} 