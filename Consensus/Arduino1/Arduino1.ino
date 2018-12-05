#include "Comm_I2C.h"
#include "consensus.h"

#define BROADCAST_ADDR 0
#define OWN_ADDR 1
#define SLAVE_ADDR 2

const int ledPin = 11;
const int sensorPin = A0;

const int R1 = 10000;
const int Vcc = 5;

const float m = -0.72;
const float b = 5.0840; // LDR (sem fita)

//String id_str = "A1: ";

/* Random K ... Calibration!
int k1 = 2;
int k2 = 1;*/
int c = 1;

float L = 150.0; // The illuminance has to be set by the user!
Consensus n1(m,b,OWN_ADDR,c,L);


void setup() {

  n1.consensus_flag = true; // One arduino has to be started with this flag to false!
  n1.calib_flag = true; // One arduino has to be started with this flag to false!
  //n1.consensus_init = true;
  Serial.begin(9600); // Increase baudrate!?
  Serial.println("<Arduino 1 is ready>");
  Wire.begin(OWN_ADDR); // Initialise as slave

  TWAR = (OWN_ADDR << 1) | 1; // Enable broadcast to be received

  //n1.msgSync(); // Wait for the two Arduinos to sync
  while(!n1.calib());
  Serial.println("Calibration complete");
  //n1.msgSync(); // Wait for the two Arduinos to sync
  Wire.onReceive(receiveEvent);

  float d1 = n1.consensusAlgorithm();
  /*analogWrite(ledPin,ceil(d1*255/100));
  readVoltage();*/
}

void loop() {
  /*
  Wire.beginTransmission(BROADCAST_ADDR);
  Wire.write("Hello word\n");
  Wire.endTransmission();
  delay(1000);
  */
  
  /*if((Serial.available() > 0)){
    String str = id_str + Serial.readString();
    Wire.beginTransmission(BROADCAST_ADDR);    
    Wire.write(str.c_str());
    Wire.endTransmission();
    Serial.println(str.c_str());
  }*/
}

void receiveEvent(int howMany){
  
  // MSG = ID + SRC_ADDR + DATA_STR
  int id;
  int src_addr;
  String data_str;

  if(Wire.available() > 0){
      id = Wire.read();
      //Serial.write(id);
      src_addr = Wire.read();
      
    while(Wire.available() > 0){
      char c = Wire.read();
      data_str += c;
    }

    n1.msgAnalyse(id, data_str);
  }
}
