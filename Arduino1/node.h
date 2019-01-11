#ifndef _NODE_H
#define _NODE_H

#include <math.h>
#include "Comm_I2C.h"
#include <Arduino.h>
#include <string.h>

#define COST_BEST 1000000
#define LOWB 50
#define UPPB 150

class Node : public Comm_I2C {

  private:
    float pwm_val;
    float u = 0;
    float L_desk;
    float L_ref;
    float c;
    float rho;
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
    volatile bool consensusCheck;

    int iter_consensus;
    int iter;

    float L;
    float m;
    float b;
    float k[MAX_LUM];
    float o;
    char v_read;

  public:

    //Node();
    Node(const float _m, const float _b, int _addr, float _c, float _rho = 0.07);

    float readVoltage();
    float readIlluminance();
    bool setPWM(int PWM);
    float extIlluminance();
    void setLux(float _L);

    bool calib();

    //~Consensus();
    void consensusAlgorithm();
    void initConsensus();

    void set_occupancy();
    void button();
    void SendInfo(int counter);

    void Read_serial(char v_read);

    void PID();
    void setupint_1();

    volatile bool Occup;

};
#endif
