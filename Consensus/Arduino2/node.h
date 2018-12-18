#ifndef _NODE_H
#define _NODE_H

#include <math.h>
#include "Comm_I2C.h"
#include <Arduino.h>
#include <string.h>


class Node : public Comm_I2C {

  private:
    float u = 0;
    float L_ref;
    float c;
    float k11; //this has to come from somewhere else... I2C!
    float k12;
    float rho;
    float y[2];
    float d_avg[2];
    float d_best[2];
    float d_out[2];
    float cost_best;
    bool checkFeasibility(float d11, float d12);
    void checkSolution(float d1_test, float d2_test);
    float getCost(float d1, float d2);
    void initConsensus(float* d_avg);
    void getCopy();
    void sendCopy(float d1, float d2);
    float Lcon;
    //float k1, ki, k2;
    //float ku = 0.035;
    float k1 = 0.01575; //atenção, depende da frequência de amostragem
    float k2 = 0.00945;
    float ki = 0.0342;
    float kwdp = 1.7;
    float T = 35.0877;
    float i_ant = 0;
    float e_ant = 0;
    float y_ant = 0;
    float usat = 0;
    float des_brightness;
    float Windup(float u);

    // FLAGS
    bool max_act;
    bool gotCopyFirst;

  protected:
    float L;
    float m;
    float b;
    float k[2];
    float o;
    float d[2];
    const int ledPin = 11;
    const int sensorPin = A0;
    const int R1 = 10000;
    const int Vcc = 5;
    volatile static bool consensusCheck;

  public:

    //Node();
    Node(const float _m, const float _b, int _addr, float _c, float _rho = 0.07);

    typedef struct NodeInfo NodeInfo;
    float readVoltage();
    float readIlluminance();
    bool setPWM(int PWM);
    float extIlluminance();
    NodeInfo* getNodeInfo();
    void setLux(float _L);

    bool calib_flag;
    const char* getConsensusData();
    bool calib();
    int msgConsensus(char id, int src_addr, String data_str);

    //~Consensus();
    void consensusAlgorithm();

    void  PID();
    void init_PID(float ku, float T);
    void set_Brightness();
    void setupint_1();

    // FLAGS
    bool consensus_init;

};
#endif