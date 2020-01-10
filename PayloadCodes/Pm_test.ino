void setupPPM()
{
  PPMserial.begin(9600);
  delay(100);
  while(!(my_hpm.stop_autosend()));
  delay(500);
  while(!(my_hpm.start_measurement()));
}
void PPMUPDATE()
{
 while(!(my_hpm.read(&p25,&p10)));
}
