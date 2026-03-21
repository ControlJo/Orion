#include <Arduino.h>

int ballrichtung;
int gelbtor;
int blautor;

// put function declarations here:
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
  Serial1.begin(9600);
  Serial2.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  doPiCommunication();
  //Serial.println("test");
}
