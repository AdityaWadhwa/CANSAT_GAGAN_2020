
void voltage()
{
  float vol;

  float cap = analogRead(A0);
  vol = (cap/1024) * 4.69 * (7.85/4.6);
}
