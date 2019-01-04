#include "Comm_I2C.h"
#include "node.h"
#include <EEPROM.h>

//#define BROADCAST_ADDR 0
//#define SLAVE_ADDR 2
//#define RASP_ADDR 4
//#define OWN_ADDR 1

// increasing the frequency of PWM signal, mask bits that are not prescale
// highest timer frequency possible
const byte mask = B11111000;
int prescale = 1;

// Loop flags and interface with the user (change their names...)
volatile bool flag2 = false;
volatile bool flag3 = false;
char v_read;
int counter = 0;

// Node information
uint8_t addr = 0;
float m = 0;
float b = 0;
int c = 1;

Node *n1_p;

void setup() {

  addr = EEPROM.read(0);
  EEPROM.get(1,m);
  EEPROM.get(5,b);
  Node n1(m, b, addr, c);
  n1_p = &n1;
    
  Wire.begin(addr); // Initialise as slave
  Wire.onReceive(receiveEvent);
  TWAR = (addr << 1) | 1; // Enable broadcast to be received

  TCCR2B = (TCCR2B & mask) | prescale; // Changing frequency of timer2

  n1_p->sayHi(); // Broadcast an "Hello World"
  Serial.begin(9600); 
  Serial.println("<Arduino is ready>");
    
  n1_p->findNodes();

  delay(1000);
  while (!n1_p->calib());
  Serial.println("Calibration complete");
  delay(1000);

  n1_p->setLux(LOWB); // Set lux reference at lower bound (50 lx)
  n1_p->initConsensus();
  n1_p->consensusAlgorithm();
  //n1_p->setupint_1();*/

}

void loop() {

  /*n1_p->set_occupancy();

  n1.consensusAlgorithm();
  //Serial.println(n1_p->readIlluminance());

  if (flag2) {
    n1_p->Read_serial(v_read);
    v_read = 'a';
    flag2 = false;
  }

  if (flag3)
  {
    counter += counter;
    Serial.println(counter);
    n1_p->SendInfo(counter);
    flag3 = false;
  }

  //delay(500);*/
}

void receiveEvent(int howMany) {

  // MSG = ID + SRC_ADDR + DATA_STR
  int id;
  int src_addr;
  String data_str;

  if (Wire.available() > 0) {
    char c = Wire.read();

    if (c == 'a') {
      n1_p->sync = true;
      return;
    }

    id = c; // not a sync message
    //Serial.println(c);
  }

  while (Wire.available() > 0) {

    src_addr = Wire.read();

    while (Wire.available() > 0) {
      char c2 = Wire.read();
      data_str += c2;
      //Serial.write(c2);
      
    }
    //Serial.write('\n');
    n1_p->msgAnalyse(id, data_str);
  }
}

void serialEvent() {
  flag2 = true;
  v_read = Serial.read();
}

ISR(TIMER1_COMPA_vect) {
  //n1_p->set_Brightness();
  n1_p->PID();
  //flag3 = true;
  /*counter = counter + 1;
    if (counter % 100 == 0)
    {
    Serial.println(counter);
    Serial.println(micros());
    }
  */
}
