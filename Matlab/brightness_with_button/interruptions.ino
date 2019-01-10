void setupint_1(){
    
  cli();  //stop interrupts
  TCCR1A = 0; // set entire TCCR1A register to 0
  TCCR1B = 0; // same for TCCR1B
  TCNT1  = 0; //initialize counter value to 0

  OCR1A = 2499;  // (16*10^6) / (freq_des*8) - 1
  TCCR1B |= (1 << WGM12); // turn on CTC mode
  TCCR1B |= (1 << CS11) | (1 << CS10); // Set prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt

  sei(); //allow interrupts
}

ISR(TIMER1_COMPA_vect){

  PID(ref_brightness, counter);
}
