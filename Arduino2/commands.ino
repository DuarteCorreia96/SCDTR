#include "node.h"

void Case_g();
void set_LUX_Ref();

String v_read;

void serialEvent() {

  v_read = Serial.readString();
  flag2 = 1;
}

void Read_serial() {

  switch (v_read[0]) {
    case 'g':
      Case_g();
      break;

    case 's':
      set_LUX_Ref();
      break;

    default:
      Serial.println("Not a valid input!");
  }

}


void Case_g() {
/*
  switch (v_read[2]) {
    case 'l':
      PrintLux();
      break;
    case 's':
      PrintState();
      break;
    default:
      Serial.println("Not a valid input!");
  } */
}

void set_LUX_Ref() {

  float new_v;
  sscanf(v_read.c_str(), "s %.3f", &new_v);
  //new_v = constrain(new_v, 0, 255);
  //ref_brightness = new_v;
  //get_initial_val();
  n1.setLux(new_v);
/*
  Serial.print("Set PWM value at: ");
  Serial.print(new_v);
  Serial.print(" -> LUX: ");
  Serial.println(ref_brightness);*/
}
