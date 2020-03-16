
void setupGPS()
{
  // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
  GPS.begin(9600);
  // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // uncomment this line to turn on only the "minimum recommended" data
  //GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);
  // For parsing data, we don't suggest using anything but either RMC only or RMC+GGA since
  // the parser doesn't care about other sentences at this time
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); // 1 Hz update rate
  // For the parsing code to work nicely and have time to sort thru the data, and
  // print it out we don't suggest using anything higher than 1 Hz

  // Request updates on antenna status, comment out to keep quiet
  GPS.sendCommand(PGCMD_ANTENNA);

  delay(1000);

  // Ask for firmware version
  GPSSerial.println(PMTK_Q_RELEASE);
}

void GPSUPDATE()
{
  if (GPS.newNMEAreceived()) 
  {
    if (GPS.parse(GPS.lastNMEA()))
    {
       GPS_time.h = GPS.hour;
       GPS_time.m = GPS.minute;
       GPS_time.s = GPS.seconds;

       if(GPS.fix)
       {
         GPS_latitude = GPS.latitude;
         GPS_longitude = GPS.longitude;
         GPS_sats = (int)GPS.satellites;
         GPS_alt = GPS.altitude;
         GPS_speed = GPS.speed;           //in knots
         GPS_angle = GPS.angle;
       }
       else
       {
         GPS_latitude = 77;
         GPS_longitude = 38;
         GPS_sats = 5;
         GPS_alt = 177;
         GPS_speed = '0';           //in knots
         GPS_angle = '0';
       }
      }
  } 
}
