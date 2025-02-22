#include <Arduino.h>

/*
Motoren festlegung:
MU = MitUhrzeigersinn (von außen) positiv
GU = GegenUhrzeigersinn           negativ
0° = vorne (vom Robo)
motor1 vorne links
motor2 vorne rechts
motor3 hinten
distance/speed ratio = cos(WinkelVonRadZuBewegungsrichtung)*(strecke die ich fahren will/speed vom geraden Rad)
*/
int wr1 = -30; // winkel von rad1 zu 0°
int wr2 = 30;
int wr3 = 90;
int zw = 0;   // Zielwinkel
int zg = 100; // Zielgeschwindigkeit

double vr1; // void maxgeschw zum fahrend der höchstegeschwindigkeit: verhältnis rad 1
double vr2;
double vr3;
double faktor = 0;

int CompIn; // void Ausrichtung
int AusrAnpassung;

double bogenmass; // weil der cosinus nur bogenmaß will-> winkel in bogenmaß umrechnen; double für mehr nachkommastellen

int ballrichtung;   //werte, die ich von der kamera bekomme
int torrichtung;

bool linie;   //wert von den lichtsensoren

int i = 0; //for fun

void setup()
{
  Serial.begin(9600); // computer usb
  Serial1.begin(9600); //pin
  Serial.println("Teensy bereit ....");
}

void GeschwindigkeitBerechnen(int Radwinkel, int Fahrwinkel, int zg) // Definition siehe oben
{
  bogenmass = (zw - Radwinkel) * 3.14159 / 180; // zw-Radwinkel gibt winkel von rad zu zielrichtung an *pi/180 ist winkel in bogenmaß
  int RadGesch = cos(bogenmass) * zg;   //berechnet die drehgeschwindigkeit
  if (Radwinkel == wr1)   //vorzeichen setzen
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

  else if (Radwinkel == wr2)
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

  else if (Radwinkel == wr3)
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

void maxgeschw()      //Das rad, dass am schnellsten drehen soll auf maximalgeschwindigkeit setzen und die anderen anpassen
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

void Ausrichtung()      //Damit der roboter immer korrekt ausgerichtet ist
{
  if (1 < CompIn && CompIn <=180)
  {
    AusrAnpassung = 50;
  }
  else if (180 < CompIn && CompIn < 359)
  {
    AusrAnpassung = -50;
  }
  vr1 = vr1 + AusrAnpassung;
  vr2 = vr2 + AusrAnpassung;
  vr3 = vr3 + AusrAnpassung;
}

void ZuBallFahren()
{
  zw = ballrichtung;
  GeschwindigkeitBerechnen(wr1, zw, zg);
  GeschwindigkeitBerechnen(wr2, zw, zg);
  GeschwindigkeitBerechnen(wr3, zw, zg);
  maxgeschw();
}

void Linie()
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
      if(0 <= torrichtung && torrichtung <= 180)
      {
        zw = 135;
      }
      if(180 < torrichtung && torrichtung <= 360)
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

void loop() // hauptmethode
{
  /*
  bool maxgesch = true;
  delay(5);
  Serial.print("Zielwinkel:");
  Serial.println(zw);
  GeschwindigkeitBerechnen(wr1, zw, zg);
  GeschwindigkeitBerechnen(wr2, zw, zg);
  GeschwindigkeitBerechnen(wr3, zw, zg);
  // Serial.println("Test");

  if (maxgesch == true)
  {
    maxgeschw();
  }
  Ausrichtung();
  Serial.print("Drehung: ");
  Serial.println(vr1 + vr2 + vr3);

  if (zw < 360)
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
  if(Serial1.available())
  {
  String receivedData = Serial1.readStringUntil('\n');
  Serial.print("Empfangen: ");
  Serial.println(receivedData);
  }
  
  switch (i)
  {
    case 0:
      Serial.println("Test            |");
      i = i + 1;
      break;
    case 1:
      Serial.println("Test        ____|____");
      i = i + 1;
      break;
    case 2:
      Serial.println("Test        |       |");
      i = i + 1;
      break;
    case 3:
      Serial.println("Test        | _ O _ |");
      i = i + 1;
      break;
    case 4:
      Serial.println("Test        |  [|]  |");
      i = i + 1;
      break;
    case 5:
      Serial.println("Test        |  | |  |");
      i = i + 1;
      break;
    case 6:
      Serial.println("Test        |_______|");
      i = i + 1;
      break;
    case 7:
      Serial.println("Test            |");
      i = i + 1;
      break;
    case 8:
      Serial.println("Test            |");
      i = i + 1;
      break;
    case 9:
      Serial.println("Test            |");
      i = i + 1;
      break;
    case 10:
      Serial.println("Test            |");
      i = 0;
      break;
  }
 delay(50);
}