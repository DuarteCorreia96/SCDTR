#include "consensus.h"

Consensus::Consensus(const float _m, const float _b, int _addr, float _c, float _rho){

	addr = _addr;
	m = _m;
	b = _b;
	c = _c;
	Lcon = L;
	rho = _rho;
	o = 0;
	iter = 1;
	L_ref = 0;
	d_avg[0] = 0; d_avg[1] = 0;
	d_best[0] = 0; d_avg[1] = 0;
	d_out[0] = 0; d_out[1] = 0;
	d[0] = 0; d[1] = 0;	
}

int Consensus::msgConsensus(char id, int src_addr, String data_str){

	/*switch (id){
		case 1:
			Serial.println("Consensus Flag -> T");
			consensus_flag = true;
			consensus_data = data_str;
			break;

		case 'L':
			d_best[src_addr-1] = atof(data_str.c_str());
			consensus_init = false;
			break;

		default:
			return -1;
			break;

	}*/
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

void Consensus::getCopy(){

	char d21_str[8];
	char d22_str[8];
	
	//Serial.print("Received: ");
	//Serial.println(consensus_data.c_str());
 
  String aux_str = consensus_data;
	char* token = strtok((char*)aux_str.c_str(), "/");
	
	if(token != NULL)	strcpy(d21_str,token);
	token = strtok(NULL,"/");
	if(token != NULL)	strcpy(d22_str,token);

	float d_aux[2];	
	d_aux[0] = atof(d21_str);
	d_aux[1] = atof(d22_str);

  /*Serial.println(d_aux[0]);
  Serial.println(d_aux[1]);*/
  
  d_out[0] = d_aux[0];
  d_out[1] = d_aux[1];
  
}

void Consensus::sendCopy(float d1, float d2){

	String str = floatToString(d2) + "/" + floatToString(d1);
	msgBroadcast(1,str);    

	//Serial.print("Sent: ");
	//Serial.println(str.c_str());
}

void Consensus::initConsensus(float* d_avg){

  msgSync();
  
  if(consensus_flag){
    delay(300);
    getCopy();
  }else{
    sendCopy(d[0],d[1]);
  }

  msgSync();

  d_avg[0] = (d[0] + d_out[0])/2;
  d_avg[1] = (d[1] + d_out[1])/2;

  Lcon = L;

  /*Serial.println("Innit started");
  
  d_best[0] = 0;
  d_best[1] = 0;
  d_avg = {0,0};
  d_best[addr-1] = L;
  y = {0,0};

  String str = "L " + floatToString((float)addr) + " " + floatToString(L);

  int error = msgBroadcast(addr,str);
  if(error != 0) Serial.println("Data not sent!");   
  
  while(consensus_init);

  Serial.println("Innit done");*/
}

float Consensus::consensusAlgorithm(){

  float y[2] = {0,0};
  float d1_m = pow(k[0],2) + pow(k[1],2);
  float d1_n = d1_m - pow(k[0],2);
  float rho_inv = 1.0/rho;
  int N_iter = 50;
  
	float d1,d2;
  int j = 0;

	y[0] = 0; // Lagrange Multipliers
	y[1] = 0;

	o = extIlluminance(); // Update external illuminance estimate
	initConsensus(d_avg);

  while (j < N_iter){

  	if(consensus_flag){

      //Serial.println(j);

      getCopy();
			consensus_flag = false;
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
			
  		    d_avg[0] = (d_best[0] + d_out[0])/2;
  		    d_avg[1] = (d_best[1] + d_out[1])/2;	    
  	
  		    y[0] += rho*(d_best[0] - d_avg[0]);
  		    y[1] += rho*(d_best[1] - d_avg[1]); 

          sendCopy(d_best[0],d_best[1]);      
          j++;

          delay(200);
  		  	continue; // No need to compute other solution, optimal solution found
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
	    d1 = rho_inv*z1 - k[0]*(o - Lcon + rho_inv*(k[0]*z1 + k[1]*z2))/d1_m;
	    d2 = rho_inv*z2 - k[1]*(o - Lcon + rho_inv*(k[0]*z1 + k[1]*z2))/d1_m;
	    checkSolution(d1, d2);
	    
	    // Solution in the ILB & DLB
			d1 = 0;
			d2 = rho_inv*z2 - (k[1]*(o - Lcon) + rho_inv*k[1]*k[1]*z2)/d1_n;
			checkSolution(d1, d2);
	    
	    // Solution in the ILB & DUB
	    d1 = 100;
	    d2 = rho_inv*z2 - (k[1]*(o - Lcon) + 100*k[1]*k[0] + rho_inv*k[1]*k[1]*z2)/d1_n;
	    checkSolution(d1, d2);
		
			// Average solutions from all nodes
			d_avg[0] = (d_best[0] + d_out[0])/2;
			d_avg[1] = (d_best[1] + d_out[1])/2;	
	    
	    // Dual update -> Update the Lagrangian Multipliers
	    y[0] += rho*(d_best[0] - d_avg[0]);
	    y[1] += rho*(d_best[1] - d_avg[1]);
	    
			sendCopy(d_best[0],d_best[1]); 	    
	    j++;

      /*Serial.println(d_best[0]);
      Serial.println(d_best[1]);*/

      delay(200);
		}
   
  }

	if(max_act){ // Request Illuminance value above LED actuation, power everthing at max
		d_best[0] = 100;
		d_best[1] = 100;
	}

	L_ref = k[0]*d_best[0]; // Value to be sent to the local controller
	Serial.println(L_ref);
	consensusCheck = true;
	Lcon = L;
}
