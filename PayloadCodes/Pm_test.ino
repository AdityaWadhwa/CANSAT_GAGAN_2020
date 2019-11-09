
ISR(TIMER2_COMPA_vect)
{
  ppm = analogRead(A6);
}

void SetupPM() {
  
  cli();
  
  pinMode(5,OUTPUT);
  pinMode(A6,INPUT);
  TCNT2 = 0;
  OCR2A = 50;
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS22) | _BV(CS21)| _BV(CS20);
  OCR2B = 60;
  TIMSK2 |= _BV(OCIE2A);

  sei();
}
