#ifndef _CONSENSUS_H
#define _CONSENSUS_H

#include <math.h>
#include "Comm_I2C.h"
#include <Arduino.h>
#include <string.h>

class Consensus
{
  private:
	  Comm_I2C *i2c;
    float L;
    float c;
    float k11; //this has to come from somewhere else... I2C!
    float k12;
    float rho;
    //float o;
    bool checkFeasibility(float d11, float d12);
    float getCost(float d11, float d12, float* y, float* d1_av);
    float* initConsensus(Comm_I2C *i2c);

  public:
    Consensus(); //default constructor
    Consensus(Comm_I2C* _i2c, float _c, float _L, float _k11, float _k12, float _rho = 0.07);
    //~Consensus();
    float consensusAlgorithm();
};

#endif
