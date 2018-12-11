#include "consensus.h"

Consensus::Consensus(const float _m, const float _b, int _addr, float _c, float _L, float _k0, float _k1, float _rho){

	addr = _addr;
  m = _m;
  b = _b;
	c = _c;
	L = _L;
	rho = _rho;
	o = 0;
	iter = 1;
	L_ref = 0;
	d_avg[0] = 0; d_avg[1] = 0;
	d_best[0] = 0; d_avg[1] = 0;
  d_out[0] = 0; d_out[1] = 0;
  d[0] = 0; d[1] = 0;
	
	k[0] = _k0;
	k[1] = _k1;
}


bool Consensus::checkFeasibility(float d1, float d2){

	float tol = 0.001;
  
  if(d1 < 0 - tol|| k[0]*d1 + k[1]*d2 < L - o - tol){
    max_act = false;
    return false;
  }
    
  if(d1 > 100 + tol){
    max_act = true;
    return false;
  }

  return true;
}

float Consensus::getCost(float d1, float d2){

  return c*d1+y[0]*(d1-d_avg[0])+y[1]*(d2-d_avg[1])+rho/2*(pow(d1-d_avg[0],2)+pow(d2-d_avg[1],2));
}

void Consensus::checkSolution(float d1_test, float d2_test){

	if(checkFeasibility(d1_test, d2_test)){

		float cost = getCost(d1_test,d2_test);
		if (cost < cost_best){

			cost_best = cost;
			d_best[0] = d1_test;
			d_best[1] = d2_test;
		}
	}
}

void Consensus::consensus_update(float *d_best, float *d_out){

    // Average solutions from all nodes
    d_avg[0] = (d_best[0] + d_out[1])/2;
    d_avg[1] = (d_best[1] + d_out[0])/2;	
    
    // Dual update -> Update the Lagrangian Multipliers
    y[0] += rho*(d_best[0] - d_avg[0]);
    y[1] += rho*(d_best[1] - d_avg[1]);
}

void Consensus::primal(){

  float y[2] = {0,0};
  float d1_m = pow(k[0],2) + pow(k[1],2);
  float d1_n = d1_m - pow(k[0],2);
  float rho_inv = 1.0/rho;
  
	float d1,d2;
  int j = 0;

	y[0] = 0; // Lagrange Multipliers
	y[1] = 0;

	o = 0;

	cost_best = 1000000; //large number

	float z1 = rho*d_avg[0] - c - y[0];
	float z2 = rho*d_avg[1] - y[1];
	
	// Unconstrained minimum
	d1 = rho_inv*z1;
	d2 = rho_inv*z2;

	if(checkFeasibility(d1,d2)){

		float cost_unconstrained = getCost(d1,d2);
		if (cost_unconstrained < cost_best){

			cost_best = cost_unconstrained;
			d_best[0] = d1;
			d_best[1] = d2;

			return; // No need to compute other solution, optimal solution found
		}
	}

	// Solution in the DLB (Dimming lower bound)
	d1 = 0;
	d2 = rho_inv*z2;
	checkSolution(d1, d2);
	
	// Solution in the DUB (Dimming Upper Bound)
	d1 = 100;
	d2 = rho_inv*z2;
	checkSolution(d1, d2);
	
	// Solution in the ILB (Illuminance Lower Bound)
	d1 = rho_inv*z1 - k[0]*(o - L + rho_inv*(k[0]*z1 + k[1]*z2))/d1_m;
	d2 = rho_inv*z2 - k[1]*(o - L + rho_inv*(k[0]*z1 + k[1]*z2))/d1_m;
	checkSolution(d1, d2);
	
	// Solution in the ILB & DLB
	d1 = 0;
	d2 = rho_inv*z2 - (k[1]*(o - L) + rho_inv*k[1]*k[1]*z2)/d1_n;
	checkSolution(d1, d2);
	
	// Solution in the ILB & DUB
	d1 = 100;
	d2 = rho_inv*z2 - (k[1]*(o - L) + 100*k[1]*k[0] + rho_inv*k[1]*k[1]*z2)/d1_n;
	checkSolution(d1, d2);

	if(max_act){ // Request Illuminance value above LED actuation, power everthing at max
		d_best[0] = 100;
		d_best[1] = 100;
	}

	return;
}
