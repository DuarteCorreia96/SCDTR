#include "consensus.h"

int main(){

  const float m = -0.72;
  const float b1 = 5.0840; 
  const float b2 = 5.0170; 
  float L = 150.0;  


//  float k1 = {2.12, 0.86};
//  float k2 = {0.58, 1.55};

  Consensus n1(m,b1,1,1,L,2.12, 0.86);
  Consensus n2(m,b2,2,1,L,0.58, 1.55);

  float d_in[2] = {50,0};
  n1.d_best[0] = d_in[0];
  n2.d_best[1] = d_in[1];
  
  float d_out[2] = {0, 50};
  n1.d_out[0] =  d_out[0];
  n2.d_out[1]=  d_out[1];

  float d_avg[2] = {25, 25};
  n1.d_avg[0] = d_avg[0];
  n2.d_avg[1] = d_avg[1];

  for(int j = 0; j < 50; j++){

    n1.primal();
    n1.consensus_update(n1.d_best, n2.d_best);
    

    n2.primal();
    n2.consensus_update(n2.d_best, n1.d_best);
  }

}