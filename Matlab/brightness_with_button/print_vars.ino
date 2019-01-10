void PrintLux(){

  Serial.print("l ");
  Serial.println(illuminance_fun(read_ldr(sensorPin)));
}

void PrintState(){

  Serial.print("s ");
  Serial.println(occup, DEC);
}
