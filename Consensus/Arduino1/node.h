#ifndef _NODE_H
#define _NODE_H

#include <math.h>
#include <Arduino.h>


class Node{

    private:

        const int ledPin = 11;
        const int sensorPin = A0;
        const int R1 = 10000;
        const int Vcc = 5;

    protected:

        float m;
        float b;
        float k[2];
        float o;
        float L_ref;
        float d[2];

    public:

        Node();
        float readVoltage();
        float readIlluminance();
        bool setPWM(int PWM);
        float extIlluminance();
};

#endif
