#include <Arduino.h>
#include <SPI.h>

#include <FlexCAN_T4.h>

const int NODE_ID_1 = 0; // TinyMOVR vorne links
const int NODE_ID_2 = 1; // TinyMOVR vorne rechts
const int NODE_ID_3 = 2; // TinyMOVR hinten

// === CAN-Objekt global anlegen ===
FlexCAN_T4<CAN2, RX_SIZE_256, TX_SIZE_16> Can2;

#define MAX_MOTORS 255
uint32_t motorIDs[MAX_MOTORS];
uint8_t foundMotors = 0;


/*
Winkel immer im uhrzeigersinn
Motoren festlegung:
MU = MitUhrzeigersinn (von außen) positiv
GU = GegenUhrzeigersinn           negativ
0° = vorne (vom Robo)
motor1 vorne links
motor2 vorne rechts
motor3 hinten
distance/speed ratio = cos(WinkelVonRadZuBewegungsrichtung)*(strecke die ich fahren will/speed vom geraden Rad)
*/
//global
int wr1 = -30; // winkel von rad1 zu 0°
int wr2 = 30;
int wr3 = 90;
int zw = 0;   // Zielwinkel
int zg = 100; // Zielgeschwindigkeit

//Geschw. Berechnen
double vr1; // void maxgeschw zum fahrend der höchstegeschwindigkeit: verhältnis rad 1
double vr2;
double vr3;
double bogenmass; // weil der cosinus nur bogenmaß will-> winkel in bogenmaß umrechnen; double für mehr nachkommastellen

//Maxgeschw
double faktor = 0;

int CompIn; // void Ausrichtung -> muss noch werte eingetragen bekommen
int AusrAnpassung;

int sl;    //sl für speedlimits ( verwendet in Drehen )

//Kamerawerte vom RPi
int ballrichtung;
int gelbtor;
int blautor;
bool BinGelb;
int gegentor;
int eigentor;
String receivedData = "1 1 1\n";

//variablen für anpassung von ausrichtung
int compnow;
int compold;
int turnspeed;
int angleoffset;
int turntime;

bool linie;   //wert von den lichtsensoren


void calibrateTinyMovr(uint8_t node_id) {
  CAN_message_t msg;
  msg.id = 0x001 + node_id;
  msg.len = 8;
  msg.buf[0] = 0x07;  // Set Axis State
  msg.buf[1] = 0x00;
  msg.buf[2] = 0x03;  // FULL_CALIBRATION_SEQUENCE
  for (int i = 3; i < 8; i++) msg.buf[i] = 0;
  Can2.write(msg);
  Serial.print("Kalibriere Node ");
  Serial.println(node_id);
  delay(5000); // Warte auf Abschluss (ca. 3–5 s)
}


void enableTinyMovr(uint8_t node_id) {
  CAN_message_t msg;
  msg.id = 0x001 + node_id;   // Adresse = 0x001 + Node-ID
  msg.len = 8;
  msg.buf[0] = 0x07;          // Befehl: "Set Axis State"
  msg.buf[1] = 0x00;
  msg.buf[2] = 0x08;          // Zielzustand: CLOSED_LOOP_CONTROL
  msg.buf[3] = msg.buf[4] = msg.buf[5] = msg.buf[6] = msg.buf[7] = 0;
  Can2.write(msg);
}

void initAllTinyMovrs() {
  calibrateTinyMovr(NODE_ID_1);
  calibrateTinyMovr(NODE_ID_2);
  calibrateTinyMovr(NODE_ID_3);
  delay(1000);
  enableTinyMovr(NODE_ID_1);
  enableTinyMovr(NODE_ID_2);
  enableTinyMovr(NODE_ID_3);
  delay(2000);
  Serial.println("Alle TinyMovrs im Closed Loop.");
}

void GeschwindigkeitBerechnen(int Radwinkel, int Fahrwinkel, int zg) // Definition siehe oben
{
  bogenmass = (zw - Radwinkel) * 3.14159 / 180; // zw-Radwinkel gibt winkel von rad zu zielrichtung an *pi/180 ist winkel in bogenmaß
  int RadGesch = cos(bogenmass) * zg;   //berechnet die drehgeschwindigkeit
  if (Radwinkel == wr1)   //vorzeichen setzen für jedes rad
  {
    if ((240 < Fahrwinkel && Fahrwinkel <= 360) || (0 <= Fahrwinkel && Fahrwinkel < 60))
    {
      RadGesch = abs(RadGesch) * (-1);
    }
    else if (60 < Fahrwinkel && Fahrwinkel < 240)
    {
      RadGesch = abs(RadGesch);
    }
    Serial.print("Rad 1:");
    Serial.println(RadGesch);
    vr1 = RadGesch;
  }

  else if (Radwinkel == wr2)    //vorzeichen setzen für jedes rad
  {
    if (120 < Fahrwinkel && Fahrwinkel < 300)
    {
      RadGesch = abs(RadGesch) * (-1);
    }
    else if ((300 < Fahrwinkel && Fahrwinkel <= 360) || (0 <= Fahrwinkel && Fahrwinkel < 120))
    {
      RadGesch = abs(RadGesch);
    }
    Serial.print("Rad 2:");
    Serial.println(RadGesch);
    vr2 = RadGesch;
  }

  else if (Radwinkel == wr3)    //vorzeichen setzen für jedes rad
  {
    if (0 < Fahrwinkel && Fahrwinkel < 180)
    {
      RadGesch = abs(RadGesch) * (-1);
    }
    else if (180 <= Fahrwinkel && Fahrwinkel <= 360)
    {
      RadGesch = abs(RadGesch);
    }
    Serial.print("Rad 3:");
    Serial.println(RadGesch);
    vr3 = RadGesch;
  }
  // Serial.println(cos(bogenmass));
}

void maxgeschw()      //Das rad, dass am schnellsten drehen soll auf maximalgeschwindigkeit gesetzt werden und die anderen anpassen
{
  if (abs(vr1) >= abs(vr2) && abs(vr1) >= abs(vr3)) // rad 1 am schnellsten
  {
    Serial.print("FaktorRad1: ");
    faktor = abs(100.0 / vr1);
  }
  else if (abs(vr2) >= abs(vr1) && abs(vr2) >= abs(vr3)) // rad 2 am schnellsten
  {
    Serial.print("FaktorRad2: ");
    faktor = abs(100.0 / vr2);
  }
  else if (abs(vr3) >= abs(vr1) && abs(vr3) >= abs(vr2)) // rad 3 am schnellsten
  {
    Serial.print("FaktorRad3: ");
    faktor = abs(100.0 / vr3);
  }
  Serial.println(faktor);
  vr1 = vr1 * faktor;
  vr2 = vr2 * faktor;
  vr3 = vr3 * faktor;
  Serial.println("Maxgesch:");
  Serial.println(vr1);
  Serial.println(vr2);
  Serial.println(vr3);
}

void getPos()
{
  
}

/*
void Ausrichtung()      //Damit der roboter immer korrekt ausgerichtet ist 180 ist hinten 0 ist vorne
{
  if (1 < CompIn && CompIn <=180)   //rechts gedreht
  {
    AusrAnpassung = 50;
  }
  else if (180 < CompIn && CompIn < 359)    //links gedreht
  {
    AusrAnpassung = -50;
  }
  vr1 = vr1 + AusrAnpassung;
  vr2 = vr2 + AusrAnpassung;
  vr3 = vr3 + AusrAnpassung;
}
*/

void ZuBallFahren()   //fährt einfach in richtung ball
{
  zw = ballrichtung;
  GeschwindigkeitBerechnen(wr1, zw, zg);
  GeschwindigkeitBerechnen(wr2, zw, zg);
  GeschwindigkeitBerechnen(wr3, zw, zg);
  maxgeschw();
}

void LiniePrüfen()
{
  int length = Serial2.available();
  for(int i = 0; i < length; i++)
  {
    int msgT = Serial2.read();
    linie = false;
  }
/*
  if(msgT != 0)   //todo
  {
    //umdrehen
    linie = true;
    zustand = 2;
    //led gelb
    Serial.println(msgT);
  }*/
}

void Linie()    //wenn linie, dann einfach umdrehen
{
  if(linie)
  {
    if((zw - 180) < 0)
    {
      zw = abs(zw - 180);
    }
    else
    {
      zw = zw - 180;
    }
  }
}

void Drehen(int drehgeschw)              //- links + rechts
{
  if (drehgeschw > 0)   // einfach drehen ohne berücksichtigen von maximalgeschwindigkeit
  {
    vr1 = vr1 - drehgeschw;
    vr2 = vr2 - drehgeschw;
    vr3 = vr3 - drehgeschw;
  }
  else if (drehgeschw < 0)
  {
    vr1 = vr1 + drehgeschw;
    vr2 = vr2 + drehgeschw;
    vr3 = vr3 + drehgeschw;
  }
  if(vr1 > 100)   //abfangen von werten > maxgesch.
  {
    vr1 = 100;
  }
  else if(vr1 < -100)
  {
    vr1 = -100;
  }

  if(vr2 > 100)
  {
    vr2 = 100;
  }
  else if(vr2 < -100)
  {
    vr2 = -100;
  }
  
  if(vr3 > 100)
  {
    vr3 = 100;
  }
  else if(vr3 < -100)
  {
    vr3 = -100;
  }
}

void BallNehmen()       //siehe Orion onion2.png 
  {
    if (320 < ballrichtung && ballrichtung < 40)
    {
      zw = ballrichtung;
    }
    else if(40 < ballrichtung && ballrichtung < 80)
    {
      zw = 90;
    }
    else if(80 < ballrichtung && ballrichtung < 100)
    {
      zw = 135;
    }
    else if(100 < ballrichtung && ballrichtung < 150)
    {
      zw = 180;
    }
    else if(150 < ballrichtung && ballrichtung < 210)   //wenn der ball hinter dem Roberter ist
    { 
      if(0 <= gegentor && gegentor <= 180)
      {
        zw = 135;
      }
      if(180 < gegentor && gegentor <= 360)
      {
        zw = 225;
      }
    }
    else if(210 < ballrichtung && ballrichtung < 260)
    {
      zw = 180;
    }
    else if(260 < ballrichtung && ballrichtung < 280)
    {
      zw = 225;
    }
    else if(280 < ballrichtung && ballrichtung < 320)
    {
      zw = 270;
    }
    GeschwindigkeitBerechnen(wr1, zw, zg);
    GeschwindigkeitBerechnen(wr2, zw, zg);
    GeschwindigkeitBerechnen(wr3, zw, zg);
    maxgeschw();
    
  }

int complesen(){}; //todo

void PID_Drive(int speed, int angle, bool max)    //in dev
{
  GeschwindigkeitBerechnen(wr1, angle, speed);
  GeschwindigkeitBerechnen(wr2, angle, speed);
  GeschwindigkeitBerechnen(wr3, angle, speed);
  if(max)
  {
    maxgeschw();
  }

  compold = compnow;
  compnow = complesen();
  turnspeed = abs(compold - compnow);
  if (compnow <= 180)    // links gedreht, negativer offset, rechts positiv
  {
    angleoffset = compnow;
  }
  else if (compnow > 180)
  {
    angleoffset == -360 + compnow;
  }
  Drehen(-angleoffset);
  if (-2 < angleoffset && angleoffset < 2) 
  {
    turntime = 0;
  }
  else
  {
    turntime = turntime + 1;
  }
  if(turntime >= 100)
  {
    // power increase?
    
  }
}

void sendSpeedToTinyMovr(uint8_t node_id, float speed) {
  CAN_message_t msg;
  msg.id = 0x00D + node_id;   // Offizielle ID für "Set Input Velocity"
  msg.len = 4;
  memcpy(msg.buf, &speed, sizeof(float));
  Can2.write(msg);
}

void SimpleDrive(int speed, int angle)
{
  // Geschwindigkeitswerte berechnen
  GeschwindigkeitBerechnen(wr1, angle, speed);
  GeschwindigkeitBerechnen(wr2, angle, speed);
  GeschwindigkeitBerechnen(wr3, angle, speed);

  // Werte an TinyMovr senden
  sendSpeedToTinyMovr(NODE_ID_1, vr1);
  sendSpeedToTinyMovr(NODE_ID_2, vr2);
  sendSpeedToTinyMovr(NODE_ID_3, vr3);

}

void doPiCommunication()
{
  if(Serial1.available())
  {
  String receivedData = Serial1.readStringUntil('\n');
  Serial.print("Empfangen: ");
  Serial.println(receivedData);
  }
  sscanf(receivedData.c_str(), "%d %d %d", &ballrichtung, &gelbtor, &blautor);
}

void findMotors()
{
  Serial.println("🔍 Scanne nach aktiven CAN-Geräten...");
  unsigned long start = millis();

  while (millis() - start < 5000) { // 5 Sekunden lang lauschen
    CAN_message_t msg;
    if (Can2.read(msg)) {  // Wenn eine Nachricht empfangen wurde
      bool known = false;
      Serial.println("nachricht gekommen");

      // Prüfen, ob diese ID schon bekannt ist
      for (uint8_t i = 0; i < foundMotors; i++) {
        if (motorIDs[i] == msg.id) known = true;
      }

      // Wenn die ID neu ist, speichern
      if (!known && foundMotors < MAX_MOTORS) {
        motorIDs[foundMotors++] = msg.id;
        Serial.printf("✅ Neuer Motor gefunden! CAN-ID: %lu (0x%03lX)\n",
        msg.id, msg.id);
      }
    }
  }

  Serial.println("\nGefundene Motoren:");
for (uint8_t i = 0; i < foundMotors; i++) {
  Serial.printf(" - Motor %u → CAN-ID: %lu (0x%03lX)\n",
  i + 1, motorIDs[i], motorIDs[i]);
}
if (foundMotors == 0) Serial.println("⚠️ Keine Motoren gefunden!");
}

void setup()
{
  Serial.begin(9600); // computer usb
  Serial1.begin(9600); //pin
  Serial2.begin(9600);

  Serial.println("starting...");
  delay(1000);
  Serial.println("los!");

  Serial.println("Teensy bereit ....");
  //while(!Serial2)
  //{
  //  Serial.println("kein boden teensy");
  //}

  // CAN Setup
  Can2.begin();
  Can2.setBaudRate(1000000); // 1 Mbit/s
  Can2.enableFIFO();
  Can2.enableFIFOInterrupt();

  Serial.println("CAN gestartet...");

  Serial.println("warte 10 sekunden");
  delay(10000);
  


  findMotors();
  Serial.println("Motoren gesucht und hoffentlich gefunden");

  // tinymovr vorbereiten
  initAllTinyMovrs();

  delay(2000); // damit der Tinymovr wirklich bereit ist

  Serial.println("jetzt gehts los");

  if(BinGelb == true)
  {
    eigentor = gelbtor;
    gegentor = blautor;
  }
  else
  {
    eigentor = blautor;
    gegentor = gelbtor;
  }
}

void loop() // hauptmethode
{
  //doPiCommunication();

  delay(2000);

  //SimpleDrive(10,0);

  findMotors();

  //Serial.println("test");

  /*
  //bool maxgesch = true;
  delay(5);
  Serial.print("Zielwinkel:");
  Serial.println(zw);
  PID_Drive(100, zw, true);
  // Serial.println("Test");

  Serial.print("Drehung: ");
  Serial.println(vr1 + vr2 + vr3);

  if (zw < 360)   // dass alle winkel durchprobiert werden
  {
    zw = zw + 1;
  }
  else
  {
    while (true)
    {
      Serial.println("fertig");
      delay(10000);
    }
  }
  */
}