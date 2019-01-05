#include "new_consensus.h"
using namespace std;

float d_test[MAX_LUM];
float z[MAX_LUM];

Consensus::Consensus(float _addr, float _c, float _L, float* _k, float _rho){

  addr = _addr;
	c = _c;
	L = _L;
	rho = _rho;
	o = 0;

  for(int j = 0; j < MAX_LUM; j++){
    d_avg[j] = 0;
    d_best[j] = 0;
	  y[j] = 0;
    k[j] = 0;
  }

  d1_m = 0;
  for(int n = 0; n < ndev; n++){
    k[n] = _k[n];
    d1_m += k[n]*k[n];
  }

  d1_n = d1_m - k[addr-1];
}

bool Consensus::checkFeasibility() {

  float tol = 0.001;

  float ill = 0;
  for(int j = 0; j < ndev; j++) ill += k[j]*d_test[j];

  max_act = false;

  if (d_test[addr-1] < 0 - tol || ill < L - o - tol)  return false;

  if (d_test[addr-1] > 100 + tol) {
    max_act = true;
    return false;
  }

  return true;
}

float Consensus::getCost() {

  int diff;

  float cost = c*d_test[addr-1];

  for(int j = 0; j < ndev; j++){
    diff = d_test[j] - d_avg[j];
    cost += ( y[j]*diff + rho/2*diff*diff );
  }

  return cost;
}

void Consensus::checkSolution(int sol) {

  if (checkFeasibility()) {
    //cout << sol << endl;
    float cost = getCost();
    if (cost < cost_best) {
      //cout << sol;
      cost_best = cost;
      for(int j = 0; j < ndev; j++) d_best[j] = d_test[j];
    }
    //cout << endl;
  }
}

void Consensus::consensus_update(float** d_out_matrix){
//void Consensus::consensus_update(const float* d_out)
  float d_out[MAX_LUM];

  float sum; // Sum all d vectors received
  for(int n = 0; n < ndev; n++){
    sum = 0;
    for(int m = 0; m < ndev; m++){
      if (m == addr-1) continue;
      sum += d_out_matrix[m][n];
    } 

    d_out[n] = sum;
  }

  for(int j = 0; j < ndev; j++){   
    d_avg[j] = (d_best[j] + d_out[j]) / ndev; // Average solutions from all nodes
    y[j] += rho*(d_best[j] - d_avg[j]); // Dual update -> Update the Lagrangian Multipliers
  }

}

void Consensus::primal(){

  float rho_inv = 1.0 / rho;
  int j = 0;
  cost_best = COST_BEST;

  for(j = 0; j < ndev; j++){

    d_best[j] = -1;

    if(j == addr-1){
      z[j] = rho*d_avg[j] - c - y[j];
      continue;
    }

    z[j] = rho*d_avg[j] - y[j];    
  }

  //Serial.println(z[0]);
  //Serial.println(z[1]);

  // Unconstrained minimum
  for(j = 0; j < ndev; j++) d_test[j] = rho_inv*z[j];
  checkSolution(1);

  // Solution in the DLB (Dimming lower bound)
  d_test[addr-1] = 0;
  checkSolution(2);

  // Solution in the DUB (Dimming Upper Bound)
  d_test[addr-1] = 100;
  checkSolution(3);

  // Solution in the ILB (Illuminance Lower Bound)
  float sum = 0;
  for(j = 0; j < ndev; j++) sum += k[j]*z[j];

  for(j = 0; j < ndev; j++) d_test[j] = rho_inv*z[j] - k[j]*(o - L + rho_inv*sum) / d1_m;
  checkSolution(4);

  // Solution in the ILB & DLB
  for(j = 0; j < ndev; j++){
    if(j == addr-1){
      d_test[j] = 0;
      continue;
    }

    d_test[j] = rho_inv*z[j] - k[j]/d1_n * (o - L - rho_inv*(k[addr-1]*z[addr-1] - sum));
  }
  checkSolution(5);

  // Solution in the ILB & DUB
  for(j = 0; j < ndev; j++){
    if(j == addr-1){
      d_test[j] = 100;
      continue;
    }

    d_test[j] = rho_inv*z[j] - (k[j]*(o - L) + 100*k[j]*k[addr-1] - rho_inv*k[j]*(k[addr-1]*z[addr-1] - sum)) / d1_n;
  }
  checkSolution(6);

  //Serial.println(d_best[0]);
  //Serial.println(d_best[1]);

  if (max_act) { // Request Illuminance value above LED actuation, power everthing at max
    for(j = 0; j < ndev; j++) d_best[j] = 100;
  }

	return;
}

void Consensus::printResults(){

  for(int j = 0; j < ndev; j++) cout << d_best[j] << endl;
}

void Consensus::printLux(){

  float L_desk = 0;
  for(int j = 0; j < ndev; j++) L_desk += k[j]*d_avg[j];
  L_desk += 0;
  cout << "-> " << addr << ". L = " << L_desk << endl;

}
