#ifndef _NODE_H
#define _NODE_H

#include <math.h>
//#include "Vector.h"
#include "Comm_I2C.h"
#include <Arduino.h>
#include <string.h>

#define COST_BEST 1000000
#define MAX_LUMM 5

class Node : public Comm_I2C {

  private:
    float pwm_val;
    float Occu;
    float u = 0;
    float L_desk;
    float L_ref;
    float c;
    float rho;
    /*Vector<float> y;
    Vector<float> d_avg;
    Vector<float> d_best;
    Vector<float> d_test;
    Vector<float> d_out;
    Vector<float> z;*/
    float y[MAX_LUM];
    float d_avg[MAX_LUM];
    float d_best[MAX_LUM];
    float d_test[MAX_LUM];
    float d_out[MAX_LUM];
    float z[MAX_LUM];
    float cost_best;
    float d1_m;
    float d1_n;
    bool checkFeasibility();
    void checkSolution();
    float getCost();
    void getCopy();
    void sendCopy();
    float Lcon;
    float k1 = 0.01575;
    float k2 = 0.00945;
    float ki = 0.0342;
    float kwdp = 0.034;
    float T = 35.0877;
    float i_ant = 0;
    float e_ant = 0;
    float y_ant = 0;
    float usat = 0;
    float des_brightness;
    float Windup(float u);

    void NodeSetup();

    // FLAGS
    bool max_act;
    bool gotCopyFirst;

    static int iter_consensus;
    static int iter;

  protected:
    float L;
    float m;
    float b;
    //Vector<float> k;
    float k[MAX_LUM];
    float o;
    //float* d;
    /*const int ledPin = 11;
    const int sensorPin = A0;
    int buttonPin = 4;
    const int R1 = 10000;
    const int Vcc = 5;*/
    volatile static bool consensusCheck;
    char v_read;

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

    const char* getConsensusData();
    bool calib();
    int msgConsensus(char id, int src_addr, String data_str);

    //~Consensus();
    void consensusAlgorithm();
    void initConsensus();

    void set_occupancy();
    void button();
    void SendInfo(int counter);

    void Read_serial(char v_read);

    void PID();
    void init_PID(float ku, float T);
    void set_Brightness();
    void setupint_1();

    // FLAGS
    bool consensus_init;

};
#endif
