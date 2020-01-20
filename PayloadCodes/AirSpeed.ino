
void AirSpeedUpdate()
{
  float cap = analogRead(A7);
  AirSpeed = (cap-512)/1024 * 100;
}
