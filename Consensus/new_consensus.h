#ifndef _NCONSENSUS_H
#define _NCONSENSUS_H

#include <math.h>
#include <iostream>

#define COST_BEST 1000000
#define MAX_LUM 5

extern int ndev;

class Consensus{

  private:
    
    int addr;
    float L;
    float c;
    float rho;
    float cost_best;
    bool checkFeasibility();
    void checkSolution(int sol); 
    float getCost();

    float d_avg[MAX_LUM];
    float y[MAX_LUM];
    float k[MAX_LUM];
    float d1_m;
    float d1_n;
    float o;

    // FLAGS
    bool max_act;

  public:
    Consensus() {};
    Consensus(float _addr, float _c, float _L, float* _k, float _rho = 0.07);
    //~Consensus();
    void primal();
    // FLAGS
    bool consensus_init;
    void consensus_update(float** d_out);
    void printResults();
    void printLux();

    float d_best[MAX_LUM];
};

#endif
