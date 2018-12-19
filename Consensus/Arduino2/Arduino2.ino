#include "Comm_I2C.h"
#include "node.h"

//#define BROADCAST_ADDR 0
//#define SLAVE_ADDR 2
#define OWN_ADDR 2

// increasing the frequency of PWM signal
// mask bits that are not prescale
//fastest possible

volatile bool flag2 = false;
const byte mask = B11111000;
int prescale = 1;
int counter = 0;

const float m = -0.72;
const float b = 5.0170; // LDR (com fita)

int c = 1;

//float L = 150.0; // The illuminance has to be set by the user!
Node n1(m, b, OWN_ADDR, c);

void setup() {
  delay(3000);
  n1.consensus_flag = false; // One arduino has to be started with this flag to false!
  n1.calib_flag = false; // One arduino has to be started with this flag to false!
  //n1.consensus_init = true;
  Serial.begin(9600); // Increase baudrate!?
  Serial.println("<Arduino 2 is ready>");
  Wire.begin(OWN_ADDR); // Initialise as slave
  Wire.onReceive(receiveEvent);

  n1.setLux(150);

  TCCR2B = (TCCR2B & mask) | prescale; // Changing frequency of timer2
  TWAR = (OWN_ADDR << 1) | 1; // Enable broadcast to be received

  //n1.msgSync(); // Wait for the two Arduinos to sync
  while (!n1.calib());
  Serial.println("Calibration complete");
  delay(1000);

  n1.initConsensus();
  
  //n1.setupint_1();
  
  /*n1.msgSync();
  n1.setPWM(255);
  delay(2000);
  Serial.println(n1.readIlluminance());*/
  
  //n1.consensusAlgorithm();
  /*analogWrite(ledPin,ceil(d1*255/100));
    readVoltage();*/
}

void loop() {
  
  n1.consensusAlgorithm();  
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


  /*if (flag2) {
    Read_serial();
    flag2 = 0;
  }

  //button();*/
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
  n1.set_Brightness();
  n1.PID();
  /*counter = counter + 1;
    if (counter % 100 == 0)
    {
    Serial.println(counter);
    Serial.println(micros());
    }
  */
}
