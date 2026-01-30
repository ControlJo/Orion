#include "controlMotor.cpp"


class MotorController{

    private:
    controlMotor motorLinks;
    controlMotor motorHinten;
    controlMotor motorRechts;

    MotorController()
       :motorLinks(1),
        motorHinten(2),
        motorRechts(3)
    {}


    void CalibrateAll() {
        motorLinks.calibrate();
        motorHinten.calibrate();
        motorRechts.calibrate();
    }


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

};