
void setupBMP() 
{                                                 
  float alt;                                      // Initialise the serial port
  bmp.begin();          
  //while(!(bmp280.getMeasurements(temperature,pressure,alt)));
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure();
  alt = bmp.readAltitude();

  if(EEPROM.read(2) - alt > 100)     //checking for in-flight reset, if altitude differs from stored ground alt by a small value then a reset had occured
  {
    EEPROM.write(2, alt);
    gnd_alt = alt;
  }
  else
    gnd_alt = EEPROM.read(2);

  Serial.print("ground alt is ");
  Serial.println(alt);
  return;
}

void BMPUPDATE() 
{
  float alt;
//  while(!(bmp280.getMeasurements(temperature,pressure,alt)));
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure();
  alt = bmp.readAltitude();
//  prev_alt = pres_alt;
  pres_alt = alt - gnd_alt; 
}
