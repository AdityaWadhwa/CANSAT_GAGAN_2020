#include "CANSAT_GAGAN.h"

SoftwareSerial GPSSerial(4,3);
Adafruit_GPS GPS(&GPSSerial);
SoftwareSerial PPMserial(6,5);
HPMA115S0 my_hpm(PPMserial);
float temperature, pressure, gnd_alt, pres_alt, prev_alt;            // Create the temperature, pressure and altitude variables
BMP280_DEV bmp280;  // Instantiate (create) a BMP280_DEV object and set-up for I2C operation
DS3231 Clock;
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time,pkt;
double vol;
struct GPS_Time
{
  int h;
  int m;
  int s;
}GPS_time;
String GPS_latitude,GPS_longitude,GPS_sats,GPS_altitude,GPS_alt,GPS_speed,GPS_angle;
char buff[100];
String Tele = "";
float pm25,pm10,ppm,AirSpeed;

int Hr_base;
int Minutes_base;
int Sec_base;

int timer1_counter;

const int CSpin = 10;
File sensorData;
int state;

void setup()
{
  Serial.begin(19200);
  Serial.println("Setup called");
  
  pinMode(8,INPUT);     //PWM to camera, not in use right now
  pinMode(A0,INPUT);    //battery volatge measurement
  pinMode(A1,INPUT);    //LDR voltage measurement
  pinMode(A7,INPUT);    //air speed sensor analog input
  
  pkt=0;
  setupGPS();
  setupBMP();
  Serial.println("Now calling RTC");
  setupRTC();
  setupSD();
  setupPPM();

  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;

  // Set timer1_counter to the correct value for our interrupt interval
  //timer1_counter = 64911;   // preload timer 65536-16MHz/256/100Hz
  //timer1_counter = 64286;   // preload timer 65536-16MHz/256/50Hz
  //timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
  timer1_counter = 3036;   // preload timer 65536-16MHz/256/1Hz
  
  TCNT1 = timer1_counter;   // preload timer
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt

  Serial.println("Setup finish");
  delay(100);

  interrupts();             // enable all interrupts

}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
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
  Tele = "5160,";
  
  pkt++;
  
  dtostrf(Ms_time, 4, 6, buff);
  Tele += buff;
  Tele += ",";
  
  dtostrf(pkt, 4, 6, buff);
  Tele += buff;
  Tele += ",";  
  
  dtostrf(pres_alt, 4, 6, buff);
  Tele += buff;
  Tele += ",";  
  
  dtostrf(pressure, 4, 6, buff);
  Tele += buff;
  Tele += ",";
    
  dtostrf(temperature, 4, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(vol, 4, 6, buff);
  Tele += buff;
  Tele += ",";

  Tele+= "," + (String)(GPS_time.h) + ":" + (String)(GPS_time.m) + ":" + (String)(GPS_time.s) + "," + GPS_latitude + "," + GPS_longitude + "," + GPS_altitude + "," + GPS_sats;

  dtostrf(AirSpeed, 4, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(state, 4, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(ppm, 4, 6, buff);
  Tele += buff;
  
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
}
