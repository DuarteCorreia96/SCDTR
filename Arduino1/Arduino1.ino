#include "Comm_I2C.h"
#include "node.h"
#include <EEPROM.h>

// increasing the frequency of PWM signal, mask bits that are not prescale
// highest timer frequency possible
const byte mask = B11111000;
int prescale = 1;

// Loop flags and interface with the user
volatile bool changeRef = false;
volatile bool sendInfo = false;
char v_read = 'a';
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
  Serial.println("<Ready>");
    
  n1_p->findNodes();

  delay(1000);
  while (!n1_p->calib());
  Serial.println('N');
  delay(1000);

  n1_p->setLux(LOWB); // Set lux reference
  n1_p->Occup = false;
  n1_p->initConsensus(); // Enable interrupts

  n1_p->setupint_1();

  // Since Consensus Algorithm does not run inside loop()...
  while(1){ 
    n1_p->consensusAlgorithm();

    // Send node's state to Raspberry   
    /*if (sendInfo){
      counter += counter;
      n1_p->SendInfo(counter);
      sendInfo = false;
    }*/

    // Change Occupancy through Serial Comm
    if (changeRef) {
      n1_p->Read_serial(v_read);
      v_read = 'a';
      changeRef = false;
    }
  }

}

void serialEvent() {
  changeRef = true;
  v_read = Serial.read();
}

void loop() {}

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
  }

  while (Wire.available() > 0) {
    
    src_addr = Wire.read();

    while (Wire.available() > 0) {
      char c2 = Wire.read();
      data_str += c2;
      
    }
    n1_p->msgAnalyse(id, src_addr, data_str);
  }
}

ISR(TIMER1_COMPA_vect) {

  n1_p->PID();
  sendInfo = true;

}
