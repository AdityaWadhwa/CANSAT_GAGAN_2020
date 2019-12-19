
void setupGPS()
{
  myserial.begin(9600);
}
void Gps()
{
  while (myserial.available()>0) 
    {        
      int comma=0;
      if(myserial.peek()==',')
       comma++;
    switch (comma){
      case 1:
      GPS_time+=myserial.read();
      break;
      case 2:
      GPS_latitude+=myserial.read();
      break;
      case 3:
      GPS_latitude+=myserial.read();
      break;
      case 4:
      GPS_longitude+=myserial.read();
      break;
      case 5:
      GPS_longitude+=myserial.read();
      break;
      case 7:
      GPS_sats+=myserial.read();
      break;
      case 9:
      GPS_altitude+=myserial.read();
      break;
      case 10:
      GPS_alt+=myserial.read();
      break;
      default:
      break;
    }
    }
}
