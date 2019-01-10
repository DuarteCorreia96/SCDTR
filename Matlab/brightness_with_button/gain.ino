float gain(){
    
  analogWrite(led1, 255);
  delay(500);
  
  float ilu_max = illuminance_fun(read_ldr(sensorPin));
  
  analogWrite(led1, 0);
  delay(500);
  
  float ext_ilu = illuminance_fun(read_ldr(sensorPin));
  float gain = (ilu_max - ext_ilu) / 255;
  
  Serial.print("\tGain = ");
  Serial.println(gain, 8);
  return gain;
}
