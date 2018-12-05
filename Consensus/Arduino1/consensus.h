#ifndef _CONSENSUS_H
#define _CONSENSUS_H

#include <math.h>
#include "Comm_I2C.h"
#include "node.h"
#include <Arduino.h>
#include <string.h>

class Consensus : public Comm_I2C {

  private:
    
    float L;
    float c;
    float k11; //this has to come from somewhere else... I2C!
    float k12;
    float rho;
    float y[2];
    float d_avg[2];
    float d_best[2];
    float cost_best;
    bool checkFeasibility(float d11, float d12);
    void checkSolution(float d1_test, float d2_test); 
    float getCost(float d1, float d2);
    void initConsensus(float* d_avg);
    float* getCopy();
    void sendCopy(float d1, float d2);

    // FLAGS
    bool max_act;


  public:
    Consensus(); //default constructor
    Consensus(const float _m, const float _b, int _addr, float _c, float _L, float _rho = 0.07);
    int msgConsensus(char id, int src_addr, String data_str);
    //~Consensus();
    float consensusAlgorithm();

    // FLAGS
    bool consensus_init;
};

#endif
