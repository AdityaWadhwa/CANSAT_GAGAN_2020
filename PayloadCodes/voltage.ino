
void voltage()
{
  float cap = analogRead(Voltage_PIN);
   vol = (cap/1024) * 4.69 * (7.85/4.6);
}
