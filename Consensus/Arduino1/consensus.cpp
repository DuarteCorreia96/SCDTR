#include "consensus.h"

Consensus::Consensus(int _addr, float _c, float _L, float _k11, float _k12, float _rho){

	addr = _addr;
	c = _c;
	L = _L;
	k11 = _k11;
	k12 = _k12;
	rho = _rho;
	o = 0;
}

int Consensus::msgConsensus(char id, int src_addr, String data_str){

	switch (id){
		case 1:
			Serial.println("Consensus Flag -> T");
			consensus_flag = true;
			consensus_data = data_str;
			break;

		case 'L':
			d_best[src_addr-1] = atof(data_str.c_str());
			consensus_init = true;
			break;

		default:
			return -1;
			break;

	}
}

bool Consensus::checkFeasibility(float d1, float d2){
	
	if(d1 < 0 || d1 > 100 || k11*d1 + k12*d2 < L - o) 
		return false;

  return true;
}

float Consensus::getCost(float d1, float d2){

	return c*d1+y[0]*(d1-d_avg[0])+y[1]*(d2-d_avg[1])+rho/2*(pow(d1-d_avg[0],2)+pow(d2-d_avg[1],2));
}


void Consensus::initConsensus(){

	Serial.println("Innit started.");
	
	d_best[0] = 0;
	d_best[1] = 0;
	d_best[addr-1] = L;

	String str = "L " + floatToString((float)addr) + " "  + floatToString(L);

	int error = msgBroadcast(addr,str);
	if(error != 0) Serial.println("Data not sent!");   
	
	while(consensus_init);

	Serial.println("Innit done.");
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

float Consensus::consensusAlgorithm(){

  float d_out[2] = {0,0}; // Local copy of d from the other node
  float y[2] = {0,0};
  float d1_m = pow(k11,2) + pow(k12,2);
  float d1_n = d1_m - pow(k11,2);
  float rho_inv = 1.0/rho;
  int N_iter = 50;
  
	float d1,d2;
  int j = 0;

	char d21_str[6];
	char d22_str[6];

	initConsensus();

  while (j < N_iter){

  	if(consensus_flag){

      Serial.print("Received: ");
      Serial.println(getConsensusData());
	  	consensus_flag = false;
	  	char* token = strtok((char*) getConsensusData(), " ");
	  	
	  	if(token != NULL)	strcpy(d21_str,token);
	  	token = strtok(NULL," ");
	  	if(token != NULL)	strcpy(d22_str,token);
	  	
	  	d_out[0] = atof(d21_str);
	  	d_out[1] = atof(d22_str);
	  	
			cost_best = 1000000; //large number
	
	    float z11 = rho*d_avg[0] - c - y[0];
	    float z12 = rho*d_avg[1] - y[1];
	    
	    // Unconstrained minimum
	    d1 = d_best[0] - rho_inv*z11;
	    d2 = d_best[1] - rho_inv*z12;
	
	    if(checkFeasibility(d1,d2)){

	      float cost_unconstrained = getCost(d1,d2);
	      if (cost_unconstrained < cost_best){

	        cost_best = cost_unconstrained;
	        d_best[0] = d1;
	        d_best[1] = d2;
			
  		    d_avg[0] = (d_best[0] + d_out[0])/2;
  		    d_avg[1] = (d_best[1] + d_out[1])/2;	    
  	
  		    y[0] += rho*(d_best[0] - d_avg[0]);
  		    y[1] += rho*(d_best[1] - d_avg[1]); 
			   
  		  	continue; // No need to compute other solution, optimal solution found
	      }
		  }
	
	    // Solution in the DLB (Dimming lower bound)
	    d1 = 0;
	  	d2 = rho_inv*z12;
			checkSolution(d1, d2);
	    
	    // Solution in the DUB (Dimming Upper Bound)
	    d1 = 100;
	    d2 = rho_inv*z12;
			checkSolution(d1, d2);
	    
	    // Solution in the ILB (Illuminance Lower Bound)
	    d1 = rho_inv*z11 - k11*(o - L + rho_inv*k11*z11)/d1_m;
	    d2 = rho_inv*z12 - k12*(o - L + rho_inv*k12*z12)/d1_m;
	    checkSolution(d1, d2);
	    
	    // Solution in the ILB & DLB
			d1 = 0;
			d2 = rho_inv*z12 - (k12*(o-L) - rho_inv*k12*k12*z12)/d1_n;
			checkSolution(d1, d2);
	    
	    // Solution in the ILB & DUB
	    d1 = 100;
	    d2 = rho_inv*z12 - (k12*(o-L) + 100*k12*k11 - rho_inv*k12*k12*z12)/d1_n;
	    checkSolution(d1, d2);
		
			// Average solutions from all nodes
			d_avg[0] = (d_best[0] + d_out[0])/2;
			d_avg[1] = (d_best[1] + d_out[1])/2;	
	    
	    // Dual update -> Update the Lagrangian Multipliers
	    y[0] += rho*(d_best[0] - d_avg[0]);
	    y[1] += rho*(d_best[1] - d_avg[1]);
	    
	    //Send copy to neighbour (I2C Class...) -> Put values into a string
	    
			String str = floatToString(d_best[0]) + " " + floatToString(d_best[1]);

      Serial.print("Sent: ");
      Serial.print(str.c_str());
	    
	    int error = msgBroadcast(1,str); // Enum for flags (perhaps), save other address somewhere... + Check errors??
      if(error != 0) Serial.println("Data not sent!");   	    
	    j++;
		}
  }
}
