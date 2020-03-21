void setupBMP() 
{                                                 
  float alt;                                      // Initialise the serial port
  bmp280.begin(BMP280_I2C_ALT_ADDR);              // Default initialisation with alternative I2C address (0x76), place the BMP280 into SLEEP_MODE 
  bmp280.setPresOversampling(OVERSAMPLING_X4);    // Set the pressure oversampling to X4
  bmp280.setTempOversampling(OVERSAMPLING_X1);    // Set the temperature oversampling to X1
  bmp280.setIIRFilter(IIR_FILTER_4);              // Set the IIR filter to setting 4
  bmp280.setTimeStandby(TIME_STANDBY_125MS);      // Set the standby time to 125 milli seconds
  bmp280.startNormalConversion();                 // Start BMP280 continuous conversion in NORMAL_MODE
  delay(200);
  
  while(!(bmp280.getMeasurements(temperature,pressure,alt)));

  if(EEPROM.read(2) - alt > 100)     //checking for in-flight reset, if altitude differs from stored ground alt by a small value then a reset had occured
  {
    EEPROM.write(2, alt);
    gnd_alt = alt;
  }
  else
    gnd_alt = EEPROM.read(2);

  Serial.print(F("ground alt is "));
  Serial.println(alt);
  /*
  do
  {
    if(bmp280.getMeasurements(temperature,pressure,alt))
    {
//      Serial.println(alt);
      gnd_alt = (gnd_alt + alt)/2;
    }
  }
  while(gnd_alt-alt < 1);
  
  Serial.print("ground alt is ");
  Serial.println(gnd_alt);
  */
  return;
}

void BMPUPDATE() 
{
  float alt;
  while(!(bmp280.getMeasurements(temperature,pressure,alt)));
  prev_alt = pres_alt;
  pres_alt = alt - gnd_alt; 
}
