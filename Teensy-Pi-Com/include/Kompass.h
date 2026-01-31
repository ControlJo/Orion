#ifndef _KOMPASS_H_
#define _KOMPASS_H_

#include <Adafruit_I2CDevice.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#define BNO055_SAMPLERATE_DELAY_MS (100)
class Kompass{
private:
    int gibZWert();
    
    Adafruit_BNO055 bno;
public:
    Kompass();
    int zWertNull;
    void resetZwert();
    bool starten();
    int zWertAuslesen();
};
#endif
