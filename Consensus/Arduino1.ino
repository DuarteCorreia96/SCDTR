#include "Comm_I2C.h"
#include "consensus.h"

#define BROADCAST_ADDR 0
#define OWN_ADDR 1
#define SLAVE_ADDR 2

const int ledPin = 11;
const int sensorPin = A0;

const int R1 = 10000;
const int Vcc = 5;

//String id_str = "A1: ";
Comm_I2C* i2c = new Comm_I2C(OWN_ADDR);

// LDR (sem fita)
float m = -0.72;
float b = 5.0840;

// Random K ... Calibration!
int k1 = 2;
int k2 = 1;
int c = 1;

Consensus n1(c,50,k1,k2);

double illuminance_fun(float v)
{
  float R2 = R1 * (Vcc - v) / v;
  double illuminance = pow(10, (log10(R2) - b) / m);

  return illuminance;
}

void readVoltage()
{
  float rate = analogRead(sensorPin);
  float rate_volts = 5 * rate / 1023;
  Serial.print("v(V) = ");
  Serial.print(rate_volts, 4);

  Serial.print("\tL(lx) = ");
  Serial.println(illuminance_fun(rate_volts));
}

void setup() {
  i2c->consensus_flag = true;
  Serial.begin(9600); // Increase baudrate!?
  Serial.println("<Arduino 1 is ready>");
  Wire.begin(OWN_ADDR); // Initialise as slave
  Wire.onReceive(receiveEvent);

  TWAR = (OWN_ADDR << 1) | 1; // Enable broadcast to be received

  /*while(Serial.available() <= 0) {};
  int L = (int) Serial.parseInt();*/

  float d1 = n1.consensusAlgorithm();
  analogWrite(ledPin,ceil(d1*255/100));
  readVoltage();
  
  delete i2c;
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
      src_addr = Wire.read();
  
    while(Wire.available() > 0){
      char c = Wire.read();
      data_str += c;
    }

    Serial.println("Got to msg Analyse!");
    n1.msgConsensus(id, src_addr, data_str);

  }
}
