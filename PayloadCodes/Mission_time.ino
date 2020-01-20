

void setupRTC() {
	// Start the I2C interface
  Serial.println("hey rtc");
	Wire.begin();
  Hr_base = Clock.getHour(h12, PM);
  Minutes_base = Clock.getMinute();
  Sec_base  = Clock.getSecond();

  if(EEPROM.read(0) - Sec_base > 60)
    EEPROM.write(0, Sec_base);
  else
    Sec_base = EEPROM.read(0);
    
	// Start the serial interface
}

void Mission_time()
{
 Ms_hours=Clock.getHour(h12, PM)-Hr_base;
 Ms_Minutes=Clock.getMinute()-Minutes_base;
 Ms_Sec=Clock.getSecond()-Sec_base;
 Ms_time=Ms_hours*60*60 + Ms_Minutes*60 + Ms_Sec;
}
