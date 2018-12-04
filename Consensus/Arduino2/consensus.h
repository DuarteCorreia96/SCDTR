#ifndef _CONSENSUS_H
#define _CONSENSUS_H

#include <math.h>
#include "Comm_I2C.h"
#include <Arduino.h>
#include <string.h>

class Consensus : protected Comm_I2C {

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
    float o; // calculate! lux_read - Kd = o
    bool checkFeasibility(float d11, float d12);
    void checkSolution(float d1_test, float d2_test); 
    float getCost(float d1, float d2);
    void initConsensus();


  public:
    Consensus(); //default constructor
    Consensus(int _addr, float _c, float _L, float _k11, float _k12, float _rho = 0.07);
    int msgConsensus(char id, int src_addr, String data_str);
    //~Consensus();
    float consensusAlgorithm();

    // FLAGS
		bool consensus_flag;
    bool consensus_init;
};

#endif
