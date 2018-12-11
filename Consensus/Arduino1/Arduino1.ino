#include "Comm_I2C.h"
#include "consensus.h"
#include "led_controller.h"

//#define BROADCAST_ADDR 0
//#define SLAVE_ADDR 2
#define OWN_ADDR 1

// increasing the frequency of PWM signal
// mask bits that are not prescale
//fastest possible

volatile bool flag2 = false;
const byte mask = B11111000;
int prescale = 1;

/*const int ledPin = 11;
  const int sensorPin = A0;

  const int R1 = 10000;
  const int Vcc = 5;*/

const float m = -0.72;
const float b = 5.0840; // LDR (sem fita)

//String id_str = "A1: ";
int c = 1;

//float L = 150.0; // The illuminance has to be set by the user!
Consensus n1(m, b, OWN_ADDR, c);
led_controller ctrl1;

void setup() {
  delay(3000);
  n1.consensus_flag = true; // One arduino has to be started with this flag to false!
  n1.calib_flag = true; // One arduino has to be started with this flag to false!
  //n1.consensus_init = true;
  Serial.begin(9600); // Increase baudrate!?
  Serial.println("<Arduino 1 is ready>");
  Wire.begin(OWN_ADDR); // Initialise as slave
  Wire.onReceive(receiveEvent);

  TCCR2B = (TCCR2B & mask) | prescale; // Changing frequency of timer2
  TWAR = (OWN_ADDR << 1) | 1; // Enable broadcast to be received

  //n1.msgSync(); // Wait for the two Arduinos to sync
  //Serial.println("Here!");
  while (!n1.calib());
  Serial.println("Calibration complete");

  n1.setLux(150);

  //float d1 = n1.consensusAlgorithm();
  /*analogWrite(ledPin,ceil(d1*255/100));
    readVoltage();*/
}

void loop() {

  /*if (counter % 1 == 0) {
    readVoltage(counter2);
    counter2++;
    }

    counter++;
    flag = 0;
    }*/

  /*if (counter2 == npoints) {
    //PrintStep();
    counter  = 0;
    counter2 = 0;
    }*/
    if (flag2) {
      Read_serial();
      flag2 = 0;
  }

  //button();
}

void receiveEvent(int howMany) {

  // MSG = ID + SRC_ADDR + DATA_STR
  int id;
  int src_addr;
  String data_str;

  if (Wire.available() > 0) {
    char c = Wire.read();
    //Serial.write(c);
    if (c == 'a') {
      n1.sync = true;
      return;
    }

    id = c; // not a sync message

    /*id = Wire.read();
      Serial.write(id);
      src_addr = Wire.read();

      while(Wire.available() > 0)
      data_str += Wire.read();

      n1.msgAnalyse(id, data_str);*/
  }

  while (Wire.available() > 0) {
    /*Serial.println(id);
      Serial.println("\n");*/
    src_addr = Wire.read();
    //Serial.println(src_addr);
    //Serial.println("\n");

    while (Wire.available() > 0) {
      char c2 = Wire.read();
      data_str += c2;
      //Serial.write(c2);
    }

    n1.msgAnalyse(id, data_str);
  }

  /*if(Wire.available() > 0){
      id = Wire.read();
      //Serial.write(id);
      src_addr = Wire.read();

    while(Wire.available() > 0)
       data_str += Wire.read();

    n1.msgAnalyse(id, data_str);
    }*/
}

ISR(TIMER1_COMPA_vect) {
  ctrl1.PID();
}
