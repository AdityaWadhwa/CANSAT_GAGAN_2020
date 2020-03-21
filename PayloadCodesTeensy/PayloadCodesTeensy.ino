#include "CANSAT_GAGAN.h"

SoftwareSerial GPSSerial(4,3);
Adafruit_GPS GPS(&GPSSerial);
SoftwareSerial PPMserial(6,5);
HPMA115S0 my_hpm(PPMserial);
float temperature, pressure, gnd_alt, pres_alt, prev_alt;             // Create the temperature, pressure and altitude variables
BMP280_DEV bmp280;                                                    // Instantiate (create) a BMP280_DEV object and set-up for I2C operation
RTClib RTC;
IntervalTimer myTimer;                                                //using a library, I don't know which timer it will use

//DS3231 Clock;
//bool Century=false;
//bool h12;
//bool PM;
//byte ADay, AHour, AMinute, ASecond, ABits;
//bool ADy, A12h, Apm;
//int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time;

int time_base, time_now;
int pkt;
float vol;

struct GPS_Time
{
  uint8_t h;
  uint8_t m;
  uint8_t s;
}GPS_time;
uint8_t GPS_latitude,GPS_longitude,GPS_sats;
int GPS_altitude;

//char buff[5];
//String Tele = "";
char* Tele;

float ppm;
uint8_t AirSpeed;

uint8_t state;
/*
int Hr_base;
int Minutes_base;
int Sec_base;

int timer1_counter;

const int CSpin = 10;
File sensorData;
*/

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Setup called"));
  
  pinMode(8,INPUT);     //PWM to camera, not in use right now
  pinMode(A0,INPUT);    //battery volatge measurement
  pinMode(A1,INPUT);    //LDR voltage measurement
  pinMode(A7,INPUT);    //air speed sensor analog input
//  pinMode(CSpin,OUTPUT);
//  digitalWrite(CSpin,HIGH);

  
  pkt=0;
  setupGPS();
  setupBMP();
  setupRTC();
  setupSD();
  setupPPM();

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  /*
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  //timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
  //timer1_counter = 3036;   // preload timer 65536-16MHz/256/1Hz
  
  TCNT1 = 3036;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  */
  myTimer.begin(myISR,1000);        //using a library, I don't know which timer it will use
  delay(100);

  Serial.println(F("exiting setup"));
  interrupts();             // enable all interrupts
}

void myISR()        // interrupt service routine 
{
  //TCNT1 = 3036;   // preload timer
  Packet();
  writeToSD();
  Serial.println(Tele);
}

void loop()
{
  Mission_time();
  voltage();
  GPSUPDATE();
  BMPUPDATE();
  PPMUPDATE();
  AirSpeedUpdate();
  update_state();
  delay(100);
}

void Packet()
{ 
  pkt++;
  
  sprintf(Tele,"5160,%d,%d,%f,%f,%f,%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f",time_now,pkt,pres_alt,pressure,temperature,vol,GPS_time.h,GPS_time.m,GPS_time.s,GPS_latitude,GPS_longitude,GPS_altitude,GPS_sats,AirSpeed,state,ppm);
  /*
  Tele = "5160,";
  //Serial.print("5160,");
  
  dtostrf(time_now, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(time_now);
  //Serial.print(",");
  
  dtostrf(pkt, 4, 6, buff);
  Tele += buff;
  Tele += ",";  
  
  //Serial.print(pkt);
  //Serial.print(",");
  
  dtostrf(pres_alt, 4, 6, buff);
  Tele += buff;
  Tele += ",";  
  
  //Serial.print(pres_alt);
  //Serial.print(",");
  
  dtostrf(pressure, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(pressure);
  //Serial.print(",");
    
  dtostrf(temperature, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(temperature);
  //Serial.print(",");
  
  dtostrf(vol, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(vol);
  //Serial.print(",");
  
  dtostrf(GPS_time.h, 4, 6, buff);
  Tele += buff;
  Tele += ":";
  
  //Serial.print(GPS_time.h);
  //Serial.print(",");
  
  dtostrf(GPS_time.m, 4, 6, buff);
  Tele += buff;
  Tele += ":";
  
  //Serial.print(GPS_time.m);
  //Serial.print(",");
  
  dtostrf(GPS_time.s, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(GPS_time.s);
  //Serial.print(",");
  
  dtostrf(GPS_latitude, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(GPS_latitude);
  //Serial.print(",");
  
  dtostrf(GPS_longitude, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(GPS_longitude);
  //Serial.print(",");
  
  dtostrf(GPS_altitude, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(GPS_altitude);
  //Serial.print(",");
  
  dtostrf(GPS_sats, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(GPS_sats);
  //Serial.print(",");
  
 // Tele+= "," + (String)(GPS_time.h) + ":" + (String)(GPS_time.m) + ":" + (String)(GPS_time.s) + "," + GPS_latitude + "," + GPS_longitude + "," + GPS_altitude + "," + GPS_sats;
  
  dtostrf(AirSpeed, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(AirSpeed);
  //Serial.print(",");
  
  dtostrf(state, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  //Serial.print(state);
  //Serial.print(",");
  
  dtostrf(ppm, 4, 6, buff);
  Tele += buff;
  
  //Serial.println(ppm);
  */
}

void update_state()
{
  /*
   0 for boot
   1 Idle
   2 Ascent
   3 Cansat Deployment
   4 Descent
   5 Payload Deployment
   6 Parachute Deployment 
   7 End
  */
  /*
   if(state==0 && pres_alt==prev_alt && pres_alt<5) //still inside rocket
      state=1;                                      //idle

   else if(state==1 && pres_alt>prev_alt)           //inside rocket
      state=2;                                      //Launch Detect

   else if(state==2 && analogRead(A1)<100)          //LDR shows cansat is outside
      state=3;                                      //Cansat Deployement 

   else if(state==3 && pres_alt<prev_alt)           //payload inside container descending
      state=4;                                      //Descent

   else if(state==4 && pres_alt<=450)               //release the payload
      state=5;                                      //Payload Deployement

   else if(state==5 && pres_alt<=100)               //release payload parachute
      state=6;

   else if(state==6 && pres_alt<=5)
      state=7;
   */
   if( (state==0 && pres_alt==prev_alt && pres_alt<5) || (state==1 && pres_alt>prev_alt) || (state==2 && analogRead(A1)<100) || (state==3 && pres_alt<prev_alt) || (state==4 && pres_alt<=450) || (state==5 && pres_alt<=100) || (state==6 && pres_alt<=5) )
      state++; 
}
