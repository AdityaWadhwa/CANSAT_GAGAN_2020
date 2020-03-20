

void setupRTC() 
{
	Wire.begin();
//  bool h12;
//  bool PM;
  Hr_base = Clock.getHour(h12, PM);
  Minutes_base = Clock.getMinute();
  Sec_base  = Clock.getSecond();

  if(EEPROM.read(0) - Sec_base > 60)    //checking for in-flight reset, if seconds base differs from stored base by a small value then a reset had occured
    EEPROM.write(0, Sec_base);
  else
    Sec_base = EEPROM.read(0);
   
}

void Mission_time()
{
  int Ms_hours,Ms_Minutes,Ms_Sec,Ms_time,pkt;
   Ms_hours=Clock.getHour(h12, PM)-Hr_base;
   Ms_Minutes=Clock.getMinute()-Minutes_base;
   Ms_Sec=Clock.getSecond()-Sec_base;
   Ms_time=Ms_hours*60*60 + Ms_Minutes*60 + Ms_Sec;
}
