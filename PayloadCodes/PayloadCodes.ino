/*Data handling code has to be rectified 
 * software code has to be written
 * air speed code has to be added 
 */
#include "CANSAT_GAGAN.h"
#define Voltage_PIN A7
//#define LDR_PIN
//#define LDR_voltage  
SoftwareSerial GPSserial(4,3);
SoftwareSerial PPMserial(6,5); // RX, TX
HPMA115S0 my_hpm(PPMserial);

BMP280_DEV bmp280;  // Instantiate (create) a BMP280_DEV object and set-up for I2C operation
DS3231 Clock;


float temperature, pressure, gnd_alt,altitude;            // Create the temperature, pressure and altitude variables
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time,pkt,ppm;
double vol;
String GPS_time,GPS_latitude,GPS_longitude,GPS_sats,GPS_altitude,GPS_alt;
char buff[100];
String Tele = "";
float p25,p10;
int Hr_base;
int Minutes_base;
int Sec_base;
int timer1_counter;
const int CSpin = 10;
File sensorData;

void setup()
{
  pkt=0;
  Serial.begin(9600);
  setupGPS();
  setupBMP();
  setupRTC();
  setupSD();

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
  BMPUPDATE();
  Mission_time();
  voltage();
  GPSUPDATE();
  PPMUPDATE();
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
  
  dtostrf(altitude, 4, 6, buff);
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
  
  Tele+=","+GPS_time+","+GPS_latitude+","+GPS_longitude+","+GPS_altitude+","+GPS_sats;

  dtostrf(pm25, 4, 6, buff);
  Tele += buff;
}

char Software_stat()
{
  /*
   0 for boot
   1 for idle 
   2 for Launch Detect
   3 for Payload Deployement
   4 for parachute deployement
   5 for Descent 
   6 for Ascent */
   if(LDR())
   return '3';
   
}

bool LDR()
{
  if(analogRead(LDR_PIN)>LDR_voltage)
  return true;
  return false;
}
