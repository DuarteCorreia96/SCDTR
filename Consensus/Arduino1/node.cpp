#include "node.h"

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

    return 5*analogRead(sensorPin)/1023;
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
