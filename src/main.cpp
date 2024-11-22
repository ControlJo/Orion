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
int wr1 = -30;          //winkel von rad1 zu 0°
int wr2 = 30;          
int wr3 = 90;
int zw = 0;            //Zielrichtung
int zg = 100;           //Zielgeschwindigkeit

double vr1;              //void maxgeschw zum fahrend der höchstegeschwindigkeit: verhältnis rad 1
double vr2;
double vr3;
double faktor = 0;

int CompIn;             //void Ausrichtung
int kompassabweichung;

double bogenmass;         //weil der cosinus nur bogenmaß will-> winkel in bogenmaß umrechnen; double für mehr nachkommastellen
void setup()
{
  Serial.begin(9600);   //computer usb
  Serial.println("Test");
}

void GeschwindigkeitBerechnen(int Radwinkel, int Fahrwinkel, int zg)    //Definition siehe oben
{
  bogenmass = (zw-Radwinkel)*3.14159/180;     //zw-Radwinkel gibt winkel von rad zu zielrichtung an *pi/180 ist winkel in bogenmaß
  int RadGesch = cos(bogenmass)*zg;    
  if(Radwinkel == wr1)
  {
    if((240 < Fahrwinkel && Fahrwinkel <= 360) || (0 <= Fahrwinkel && Fahrwinkel < 60))
    {
      RadGesch = abs(RadGesch)*(-1);
    }
    else if(60 < Fahrwinkel && Fahrwinkel < 240)
    {
      RadGesch = abs(RadGesch);
    }
    Serial.print("Rad 1:");
    Serial.println(RadGesch);
    vr1 = RadGesch;             
  }

  else if(Radwinkel == wr2)
  {
    if(120 < Fahrwinkel && Fahrwinkel < 300)
    {
      RadGesch = abs(RadGesch)*(-1);
    }
    else if((300 < Fahrwinkel && Fahrwinkel<= 360) || (0 <= Fahrwinkel && Fahrwinkel < 120))
    {
      RadGesch = abs(RadGesch);
    }
    Serial.print("Rad 2:");
    Serial.println(RadGesch);
    vr2 = RadGesch;             
  }

   else if(Radwinkel == wr3)
  {
    if(0 < Fahrwinkel && Fahrwinkel < 180)
    {
      RadGesch = abs(RadGesch)*(-1);
    }
    else if(180 <= Fahrwinkel && Fahrwinkel <= 360)
    {
      RadGesch = abs(RadGesch);
    }
    Serial.print("Rad 3:");
    Serial.println(RadGesch);
    vr3 = RadGesch;          
  }
  //Serial.println(cos(bogenmass));
}

void maxgeschw()
{
 if(abs(vr1) >= abs(vr2) && abs(vr1) >= abs(vr3))        //rad 1 am schnellsten
  {
   Serial.print("FaktorRad1: ");
   faktor = abs(100.0 / vr1);
  }
  else if(abs(vr2) >= abs(vr1) && abs(vr2) >= abs(vr3))        //rad 2 am schnellsten
  {
   Serial.print("FaktorRad2: ");
   faktor = abs(100.0 / vr2);
  }
  else if(abs(vr3) >= abs(vr1) && abs(vr3) >= abs(vr2))        //rad 3 am schnellsten
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

void Ausrichtung()
{
  if(CompIn <= 180)
  {
   kompassabweichung = CompIn;
  }
  else
  {
   kompassabweichung = -360 + CompIn;
  }
  vr1 = vr1 + kompassabweichung;
  vr2 = vr2 + kompassabweichung;
  vr3 = vr3 + kompassabweichung;
}

void loop()                   //hauptmethode
{
 bool maxgesch = true;
 delay(5);
 Serial.print("Zielwinkel:");
 Serial.println(zw);
 GeschwindigkeitBerechnen(wr1,zw,zg);
 GeschwindigkeitBerechnen(wr2,zw,zg);
 GeschwindigkeitBerechnen(wr3,zw,zg);
 //Serial.println("Test");


 if(maxgesch == true)
 {
  maxgeschw();
 }

 Ausrichtung();
 Serial.print("Drehung: ");
 Serial.println(vr1+vr2+vr3);

 if(zw < 360 )
 {
  zw = zw + 1;
 }
 else
 {
  while(true)
  {
   Serial.println("fertig");
   delay(10000);
  }
 }
}