float temperature, pressure, gnd_alt,altit;            // Create the temperature, pressure and altitude variables
bool Century=false;
bool h12;
bool PM;
byte ADay, AHour, AMinute, ASecond, ABits;
bool ADy, A12h, Apm;
int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time,pkt;
double vol;
double GPS_time,GPS_latitude,GPS_longitude,GPS_sats,GPS_altitude,GPS_alt;
char buff[100];
String Tele = "";
float pm25,pm10,ppm,AirSpeed;

int Hr_base;
int Minutes_base;
int Sec_base;

int timer1_counter;

const int CSpin = 10;
//File sensorData;

int GPS_sat_array[] = {6,7};

void setup()
{
  Serial.begin(9600);
  Serial.println("Setup called");
  
  pinMode(8,INPUT);     //PWM to camera, not in use right now
//  pinMode(A0,INPUT);    //battery volatge measurement
//  pinMode(A1,INPUT);    //LDR voltage measurement
//  pinMode(A7,INPUT);    //air speed sensor analog input
  
  pkt=0;
  /*
  setupGPS();
  setupBMP();
  Serial.println("Now calling RTC");
  setupRTC();
//  setupSD();
  setupPPM();
  */
  Ms_time=0;
  altit=0;
  pressure=997;
  temperature=20.532;
  ppm=173;
  AirSpeed=0.2;
  vol = 8.7;
  GPS_latitude=27.2038;
  GPS_longitude=77.5011;
  GPS_altitude=158;
  GPS_sats=6;                                 //expected values

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
  
  randomSeed(analogRead(A2));
  
  Serial.println("Setup finish");
  delay(100);

  interrupts();             // enable all interrupts

}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
  TCNT1 = timer1_counter;   // preload timer
  Ms_time     += 1;
  GPS_time += 1;
  Packet();
//  writeToSD();
  Serial.println(Tele);
}

void loop()
{
  /*
  Mission_time();
  voltage();
  GPSUPDATE();
  BMPUPDATE();
  PPMUPDATE();
  AirSpeedUpdate();
  */
  altit       += 0.01*pow((-1),random(0,2))*random(0,10); 
  pressure    += 0.01*pow((-1),random(0,2))*random(0,10);
  temperature += 0.01*pow((-1),random(0,2))*random(0,10);
  ppm         += 0.01*pow((-1),random(0,2))*random(0,10);
  AirSpeed    += 0.01*pow((-1),random(0,2))*random(0,10);
  vol         -= 0.00001*random(0,10);
  GPS_latitude+= 0.0001*pow((-1),random(0,2))*random(0,10);
  GPS_longitude+= 0.0001*pow((-1),random(0,2))*random(0,10);
  GPS_altitude+= 0.01*pow((-1),random(0,2))*random(0,10);
  GPS_sats    = GPS_sat_array[random(0,2)]; 

  delay(100);
}

void Packet()
{ 
  Tele = "5160,";
    
  pkt++;
  
  dtostrf(Ms_time, 8, 6, buff);
  Tele += buff;
  Tele += ",";
  
  dtostrf(pkt, 8, 6, buff);
  Tele += buff;
  Tele += ",";  
  
  dtostrf(altit, 8, 6, buff);
  Tele += buff;
  Tele += ",";  
  
  dtostrf(pressure, 8, 6, buff);
  Tele += buff;
  Tele += ",";
    
  dtostrf(temperature, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(vol, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(GPS_time, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(GPS_latitude, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(GPS_longitude, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(GPS_altitude, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(GPS_sats, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  dtostrf(AirSpeed, 8, 6, buff);
  Tele += buff;
  Tele += ",";
  
  Tele += "1";  //software state
  Tele += ",";

  dtostrf(ppm, 8, 6, buff);
  Tele += buff;
  Tele += ",";

  Tele += "1";  //bonus direction
}
