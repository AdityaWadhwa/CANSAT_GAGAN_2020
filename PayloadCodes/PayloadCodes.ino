#include "CANSAT_GAGAN.h"
#define pin A7

float temperature, pressure, gnd_alt=0.0,altitude;            // Create the temperature, pressure and altitude variables
BMP280_DEV bmp280;  // Instantiate (create) a BMP280_DEV object and set-up for I2C operation
DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time,pkt,ppm;
double vol;
char buff[100];
String Tele = "";

int Hr_base;
int Minutes_base;
int Sec_base;

void setup()
{
  pkt=0;
  Serial.begin(9600);
  setupBMP();
  setupRTC();
  delay(1000);
}

void loop()
{
  BMPUPDATE();
  Mission_time();
  voltage();
  Serial.println(altitude);
  Packet();
  delay(100);
}

void Packet()
{ 
  Tele = "GAGAN1,";
  
  pkt++;
  
  dtostrf(Ms_time, 4, 6, buff);
  Tele += buff;
  Tele += " ";
  
  dtostrf(pkt, 4, 6, buff);
  Tele += buff;
  Tele += " ";  
  
  dtostrf(altitude, 4, 6, buff);
  Tele += buff;
  Tele += " ";  
  
  dtostrf(pressure, 4, 6, buff);
  Tele += buff;
  Tele += " ";
    
  dtostrf(temperature, 4, 6, buff);
  Tele += buff;
  Tele += " ";

  dtostrf(vol, 4, 6, buff);
  Tele += buff;
  
  Serial.println(Tele);
}
