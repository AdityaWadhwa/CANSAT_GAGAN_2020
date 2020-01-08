
void setupGPS()
{
  GPSserial.begin(9600);
}
void GPSUPDATE()
{
  while (GPSserial.available()>0) 
    {    
      Serial.println("GPS SERIAL is available");    
      int comma=0;
      if(GPSserial.peek()==',')
       comma++;
    switch (comma){
      case 1:
      GPS_time+=GPSserial.read();
      break;
      case 2:
      GPS_latitude+=GPSserial.read();
      break;
      case 3:
      GPS_latitude+=GPSserial.read();
      break;
      case 4:
      GPS_longitude+=GPSserial.read();
      break;
      case 5:
      GPS_longitude+=GPSserial.read();
      break;
      case 7:
      GPS_sats+=GPSserial.read();
      break;
      case 9:
      GPS_altitude+=GPSserial.read();
      break;
      case 10:
      GPS_alt+=GPSserial.read();
      break;
      default:
      break;
    }
    }
}
