float read_ldr(int r_sensorPin) {

  float rate = analogRead(r_sensorPin);
  float rate_volts = 5 * rate / 1023;
  return rate_volts;
}

float illuminance_fun(float v) {

  float R2 = R1 * (Vcc - v) / v;
  float illuminance = pow(10, (log10(R2) - b) / m);

  return illuminance;
}
