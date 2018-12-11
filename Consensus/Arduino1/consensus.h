#ifndef _CONSENSUS_H
#define _CONSENSUS_H

#include <math.h>
#include "Comm_I2C.h"
#include "node.h"
#include <string.h>

using namespace std;

class Consensus : public Comm_I2C {

  private:
    
    float L;
    float c;
    float rho;
    float y[2];
    float cost_best;
    bool checkFeasibility(float d11, float d12);
    void checkSolution(float d1_test, float d2_test); 
    float getCost(float d1, float d2);


    // FLAGS
    bool max_act;
    bool gotCopyFirst;


  public:
    Consensus(); //default constructor
    Consensus(const float _m, const float _b, int _addr, float _c, float _L, float _k0, float _k1, float _rho = 0.07);
    //~Consensus();
    void primal();

    float d_out[2];
    float d_avg[2];
    float d_best[2];
    // FLAGS
    bool consensus_init;

    void consensus_update(float *d_best, float *d_out);
};

#endif
