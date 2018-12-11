#include "node.h"

typedef struct NodeInfo{
  float m;
  float b;
  float k1;
  float k2;
  float L;
  float d1;
  float d2;
} NodeInfo;

Node::Node(){
    
    d[0] = 0; // Duty-cycle set by the local controller
    d[1] = 0;
};

float Node::readIlluminance(){

    float v = readVoltage();
    float R2 = R1 * (Vcc - v) / v;
    float illuminance = pow(10, (log10(R2) - b) / m);

    return illuminance;
}

float Node::readVoltage(){
  
    return 5*analogRead(sensorPin)/1023.0;
}

bool Node::setPWM(int PWM){

    if(PWM < 0 || PWM > 255) 
        return false;

    analogWrite(ledPin,PWM);
    return true;
}


float Node::extIlluminance(){

    o = readIlluminance() - k[0]*d[0] - k[1]*d[1];
    return o;
}

NodeInfo* Node::getNodeInfo(){
  
  NodeInfo* n;
  n->m = m;
  n->b = b;
  n->L = L_ref;
  
  n->k1 = k[0];
  n->k2 = k[1];
  n->d1 = d[0];
  n->d2 = d[1];

  return n;
}

void Node::setLux(float _L){
  L = _L;
}
