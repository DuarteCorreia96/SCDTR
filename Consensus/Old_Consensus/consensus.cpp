#include "consensus.h"
using namespace std;

Consensus::Consensus(float _c, float _L, float _k0, float _k1, float _rho){

	c = _c;
	L = _L;
	rho = _rho;
	o = 0;
	d_avg[0] = 0; d_avg[1] = 0;
 	d_out[0] = 0; d_out[1] = 0;
	y[0] = 0; y[1] = 0;
	
	k[0] = _k0;
	k[1] = _k1;

	d1_m = pow(k[0],2) + pow(k[1],2);
  d1_n = d1_m - pow(k[0],2);
}

bool Consensus::checkFeasibility(float d1, float d2){

	float tol = 0.001;

  max_act = false;
  
  if(d1 < 0 - tol|| k[0]*d1 + k[1]*d2 < L - o - tol)  return false;
    
  if(d1 > 100 + tol){
    max_act = true;
    return false;
  }

  return true;
}

float Consensus::getCost(float d1, float d2){

  return c*d1+y[0]*(d1-d_avg[0])+y[1]*(d2-d_avg[1])+rho/2*(pow(d1-d_avg[0],2)+pow(d2-d_avg[1],2));
}

void Consensus::checkSolution(float d1_test, float d2_test, int sol){

	if(checkFeasibility(d1_test, d2_test)){
    //cout << sol << endl;
		float cost = getCost(d1_test,d2_test);
		if (cost < cost_best){
      //cout << sol;
			cost_best = cost;
			d_best[0] = d1_test;
			d_best[1] = d2_test;
		}
    //cout << endl;
	}
}

void Consensus::consensus_update(float* d_out){

    // Average solutions from all nodes
    d_avg[0] = (d_best[0] + d_out[0])/2;
    d_avg[1] = (d_best[1] + d_out[1])/2;	
    
    // Dual update -> Update the Lagrangian Multipliers
    y[0] += rho*(d_best[0] - d_avg[0]);
    y[1] += rho*(d_best[1] - d_avg[1]);
}

void Consensus::primal(){

    //unsigned long init = micros();
  float rho_inv = 1.0 / rho;
  float d1, d2;
  cost_best = COST_BEST;
  d_best[0] = -1;
  d_best[1] = -1;
 
  float z1 = rho * d_avg[0] - c - y[0];
  float z2 = rho * d_avg[1] - y[1];

  // Unconstrained minimum
  d1 = rho_inv*z1;
  d2 = rho_inv*z2;
  checkSolution(d1,d2,1);

  // Solution in the DLB (Dimming lower bound)
  d1 = 0;
  d2 = rho_inv * z2;
  checkSolution(d1, d2,2);

  // Solution in the DUB (Dimming Upper Bound)
  d1 = 100;
  d2 = rho_inv * z2;
  checkSolution(d1, d2,3);

  // Solution in the ILB (Illuminance Lower Bound)
  d1 = rho_inv * z1 - k[0] * (o - L + rho_inv * (k[0] * z1 + k[1] * z2)) / d1_m;
  d2 = rho_inv * z2 - k[1] * (o - L + rho_inv * (k[0] * z1 + k[1] * z2)) / d1_m;
  checkSolution(d1, d2,4);

  // Solution in the ILB & DLB
  d1 = 0;
  d2 = rho_inv * z2 - (k[1] * (o - L) + rho_inv * k[1] * k[1] * z2) / d1_n;
  checkSolution(d1, d2,5);

  // Solution in the ILB & DUB
  d1 = 100;
  d2 = rho_inv * z2 - (k[1] * (o - L) + 100 * k[1] * k[0] + rho_inv * k[1] * k[1] * z2) / d1_n;
  checkSolution(d1, d2,6);

  if (max_act) { // Request Illuminance value above LED actuation, power everthing at max
    d_best[0] = 100;
    d_best[1] = 100;
  }

	d_out[0] = d_best[1];
	d_out[1] = d_best[0];

	return;
}

void Consensus::printResults(){

	cout << d_best[0] << endl;
	cout << d_best[1] << endl;
}
