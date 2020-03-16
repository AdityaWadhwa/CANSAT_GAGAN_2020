#include "CANSAT_GAGAN.h"
#define BURNPIN 30  // define it 
#define demand_allocation 20  // Calculted time for burinig mechanism

SoftwareSerial GPSSerial(4,3);
Adafruit_GPS GPS(&GPSSerial);

SoftwareSerial PPMserial(6,5);
HPMA115S0 my_hpm(PPMserial);

volatile float temperature, pressure, gnd_alt, pres_alt, prev_alt;            // Create the temperature, pressure and altitude variables
BMP280_DEV bmp280;                                                            // Instantiate (create) a BMP280_DEV object and set-up for I2C operation


DS3231 Clock;
volatile bool Century=false;
volatile bool h12;
volatile bool PM;
volatile byte ADay, AHour, AMinute, ASecond, ABits;
volatile bool ADy, A12h, Apm;
volatile int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time,pkt;


volatile double vol; // voltage

struct GPS_Time
{
  int h;
  int m;
  int s;
}GPS_time;

volatile String GPS_latitude,GPS_longitude,GPS_sats,GPS_altitude,GPS_alt,GPS_speed,GPS_angle;


volatile char buff[100];
volatile String Tele = "";
volatile float pm25,pm10,ppm,AirSpeed;

volatile int Hr_base;
volatile int Minutes_base;
volatile int Sec_base;

int timer1_counter;

const int CSpin = 10;
File sensorData;
volatile int state;

bool demand;          //handles deactivation demand of burining mechanism
int demand_cycles;    // demand cycle counter
void setup()
{
  Serial.begin(19200);
  Serial.println("Setup called");
  
  pinMode(8,INPUT);     //PWM to camera, not in use right now
  pinMode(A0,INPUT);    //battery volatge measurement
  pinMode(A1,INPUT);    //LDR voltage measurement
  pinMode(A7,INPUT);    //air speed sensor analog input

  demand = false;
  demand_cycles=0
  pkt=0;   // setup continuity of packets to be done 
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
  if(!(demand_cycles - demand_allocation)&&demand)   // deactivates burning mechanism after demand is generated 
      payload_burn_stop();
   else
       demand_cycles++;
  Serial.println(Tele);
}
//the list of sensors is
//1 PPM HPMA115S0 **
//2 BMP 280**
//3 Air Speed MPX....**
//4 RTC DS3231**
//5 GPS 
//6 Voltage Measurement **
//7 Servos Parachute Release 
//8 Burning Mechanism 
//9 state Update 
//10 Packet**
void payload_burn_start()
{
  digitalWrite(BURNPIN, HIGH);
}
void payload_burn_stop()
{
  {
  digitalWrite(BURNPIN, LOW);
}
}
void Servo_parachute();
void loop()
{
  Mission_time();     //rtc
  voltage();          //voltage divider
  GPSUPDATE();        //gps
  BMPUPDATE();        //pressure and temperature 
  PPMUPDATE();        // Particulate sensor HPMA115s0**
  AirSpeedUpdate();   // Air speed **
  update_state();     // yet toe decided 
  delay(100);
}
