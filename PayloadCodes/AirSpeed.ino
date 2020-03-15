
void AirSpeedUpdate()
{
  int sensorData = analogRead(A7);
  float pressure = map(sensorData,102,921,-2000,2000);
  
  AirSpeed = sqrt(2*pressure/1.225) - 17;
}
