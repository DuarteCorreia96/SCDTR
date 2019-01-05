#ifndef _CONSENSUS_H
#define _CONSENSUS_H

#include <math.h>
#include <iostream>

#define COST_BEST 1000000

class Consensus{

  private:
    
    float L;
    float c;
    float rho;
    float cost_best;
    bool checkFeasibility(float d11, float d12);
    void checkSolution(float d1_test, float d2_test, int sol); 
    float getCost(float d1, float d2);

    float d_avg[2];
    float d_best[2];
    float y[2];
    float k[2];
    float d1_m;
    float d1_n;
    float o;

    // FLAGS
    bool max_act;
    //bool gotCopyFirst;

  public:
    Consensus() {};
    Consensus(float _c, float _L, float _k0, float _k1, float _rho = 0.07);
    //~Consensus();
    void primal();
    // FLAGS
    bool consensus_init;
    void consensus_update(float* d_out);
    void printResults();

    float d_out[2];
};

#endif
