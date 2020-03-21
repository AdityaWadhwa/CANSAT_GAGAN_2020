void setupPPM()
{
  PPMserial.begin(9600);
  //Serial.println (F("HPMA115S0 software serial example"));

  delay(100);
 
   // Stop autosend
  int my_status = my_hpm.stop_autosend(); 
  if (my_status == 1)
  {
    //Serial.println(F("Autosend disabled"));
  }
  else
  {
    Serial.print(F("Error"));  
  }
  //delay(500);
  
  // Start fan (and measurement mode)
  my_status = my_hpm.start_measurement();  
  if (my_status == 1)
  {
    //Serial.println(F("Start Particle Measurement"));
  }
  else
  {
    Serial.print(F("Error"));  
  }
  //delay(5000);
}
void PPMUPDATE()
{
  float pm25,pm10;
  if(my_hpm.read(&pm25,&pm10))
        ppm = pm25;
  //else
  //      ppm = ppm;  //preserve last reading,don't need to write this
}
