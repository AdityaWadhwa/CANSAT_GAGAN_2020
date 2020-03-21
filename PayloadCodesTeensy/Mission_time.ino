

void setupRTC() 
{
	// Start the I2C interface
  //Serial.println("hey rtc");
	//Wire.begin();
  //Hr_base = Clock.getHour(h12, PM);
  //Minutes_base = Clock.getMinute();
  //Sec_base  = Clock.getSecond();
  
  DateTime now = RTC.now();
  time_base = now.hour()*3600 + now.minute()*60 + now.second();
  
  if(EEPROM.read(0) - time_base > 60)    //checking for in-flight reset, if seconds base differs from stored base by a small value then a reset had occured
    EEPROM.write(0, time_base);
  else
    time_base = EEPROM.read(0);
    
	// Start the serial interface
}

void Mission_time()
{
  /*
   Ms_hours=Clock.getHour(h12, PM)-Hr_base;
   Ms_Minutes=Clock.getMinute()-Minutes_base;
   Ms_Sec=Clock.getSecond()-Sec_base;
   Ms_time=Ms_hours*60*60 + Ms_Minutes*60 + Ms_Sec;
  */
  DateTime now = RTC.now();
  time_now = now.hour()*3600 + now.minute()*60 + now.second();

  time_now -= time_base;
  
}
