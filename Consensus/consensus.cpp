#include "consensus.h"

Consensus::Consensus(Comm_I2C* _i2c, float _c, float _L, float _k11, float _k12, float _rho)
{
    i2c = _i2c;
	  c = _c;
    L = _L;
    k11 = _k11;
    k12 = _k12;
    rho = _rho;

}

bool Consensus::checkFeasibility(float d11, float d12)
{
	int o = 0; // Again!!
	
	if(d11 < 0) return false;
    if(d11 > 100) return false;
    if(k11*d11 + k12*d12 < L - o) return false;
    
    return true;
}

float Consensus::getCost(float d11, float d12, float* y, float* d1_av)
{
	return c*d11+y[0]*(d11-d1_av[0])+y[1]*(d12-d1_av[1])+rho/2*(pow(d11-d1_av[0],2)+pow(d12-d1_av[1],2));
}


float Consensus::consensusAlgorithm()
{
  float d1[2] = {0,0};
  float d2[2] = {0,0}; // Local copy of d from the other node
  float d1_av[2] = {0,0};
  float y[2] = {0,0};
  float o = 0; // calculate! lux_read - Kd = o
  float d1_m = pow(k11,2) + pow(k12,2);
  float d1_n = d1_m - pow(k11,2);
  float rho_inv = 1.0/rho;
  int N_iter = 50;
  
  int j = 0;
  
  while (j < N_iter)
  {
  	if(i2c->consensus_flag)
  	{
      Serial.print("Received from the other node: ");
      Serial.println(i2c->getConsensusData());
	  	i2c->consensus_flag = false;
	  	char* token = strtok((char*)i2c->getConsensusData(), " ");
	  	
	  	char d21_str[6];
	  	char d22_str[6];
	  	
	  	if(token != NULL)	strcpy(d21_str,token);
	  	token = strtok(NULL," ");
	  	if(token != NULL)	strcpy(d22_str,token);
	  	
	  	d2[0] = atof(d22_str);
	  	d2[1] = atof(d21_str);
	  	
	  	// Measure external illuminance!!
		  
	    float d11_best = -1;
	    float d12_best = -1;
	    float cost_best = 1000000; //large number
	
	    float z11 = rho*d1_av[0] - c - y[0];
	    float z12 = rho*d1_av[1] - y[1];
	    
	    // Unconstrained minimum
	    float d11_u = d1[0] - rho_inv*z11;
	    float d12_u = d1[1] - rho_inv*z12;
	
	    if(checkFeasibility(d11_u,d12_u))
	    {
	      float cost_unconstrained = getCost(d11_u,d12_u,y,d1_av);
	      if (cost_unconstrained < cost_best)
	      {
	        cost_best = cost_unconstrained;
	        d11_best = d11_u;
	        d12_best = d12_u;
	               
  		    d1[0] = d11_best;
  			  d1[1] = d12_best;
			
  		    d1_av[0] = (d1[0] + d2[0])/2;
  		    d1_av[1] = (d1[1] + d2[1])/2;	    
  	
  		    y[0] += rho*(d1[0] - d1_av[0]);
  		    y[1] += rho*(d1[1] - d1_av[1]); 
			   
  		  	continue; // No need to compute other solution, optimal solution found
	      }
	
		  }
	
	    // Solution in the DLB (Dimming lower bound)
	    float d11_b0 = 0;
	    float d12_b0 = rho_inv*z12;
	
	    if(checkFeasibility(d11_b0,d12_b0))
	    {
	      float cost_b_0 = getCost(d11_b0,d12_b0,y,d1_av);
	      if (cost_b_0 < cost_best)
	      {
	        cost_best = cost_b_0;
	        d11_best = d11_b0;
	        d12_best = d12_b0;
	      }
	    }
	    
	    // Solution in the DUB (Dimming Upper Bound)
	    float d11_b100 = 100;
	    float d12_b100 = rho_inv*z12;
	    
	    if(checkFeasibility(d11_b100,d12_b100))
	    {
	      float cost_b_100 = getCost(d11_b100,d12_b100,y,d1_av);
	      if (cost_b_100 < cost_best)
	      {
	        cost_best = cost_b_100;
	        d11_best = d11_b100;
	        d12_best = d12_b100;
	      }
	    }
	    
	    // Solution in the ILB (Illuminance Lower Bound)
	    float d11_bl = rho_inv*z11 - k11*(o - L + rho_inv*k11*z11)/d1_m;
	    float d12_bl = rho_inv*z12 - k12*(o - L + rho_inv*k12*z12)/d1_m;
	    
	    if(checkFeasibility(d11_bl,d12_bl))
	    {
	      float cost_bl = getCost(d11_bl,d12_bl,y,d1_av);
	      if (cost_bl < cost_best)
	      {
	        cost_best = cost_bl;
	        d11_best = d11_bl;
	        d12_best = d12_bl;
	      }
	    }
	    
	    // Solution in the ILB & DLB
		float d11_l0 = 0;
		float d12_l0 = rho_inv*z12 - (k12*(o-L) - rho_inv*k12*k12*z12)/d1_n;
		
	    if(checkFeasibility(d11_l0,d12_l0))
	    {
	      float cost_l_0 = getCost(d11_l0,d12_l0,y,d1_av);
	      if (cost_l_0 < cost_best)
	      {
	        cost_best = cost_l_0;
	        d11_best = d11_l0;
	        d12_best = d12_l0;
	      }
	    }
	    
	    // Solution in the ILB & DUB
	    float d11_l100 = 100;
	    float d12_l100 = rho_inv*z12 - (k12*(o-L) + 100*k12*k11 - rho_inv*k12*k12*z12)/d1_n;
	    
	    if(checkFeasibility(d11_l100,d12_l100))
	    {
	      float cost_l_100 = getCost(d11_l100,d12_l100,y,d1_av);
	      if (cost_l_100 < cost_best)
	      {
	        cost_best = cost_l_100;
	        d11_best = d11_l100;
	        d12_best = d12_l100;
	      }
	    }
	    
	    // Save dimming values for the next cycle
	    d1[0] = d11_best;
		 d1[1] = d12_best;
		
		// Average solutions from all nodes
	    d1_av[0] = (d1[0] + d2[0])/2;
	    d1_av[1] = (d1[1] + d2[1])/2;
	    
	    // Dual update -> Update the Lagrangian Multipliers
	    y[0] += rho*(d1[0] - d1_av[0]);
	    y[1] += rho*(d1[1] - d1_av[1]);
	    
	    //Send copy to neighbour (I2C Class...) -> Put values into a string
	    float* d1_p = d1;
	    
	    char str_aux[6];
	    char d1_send[12];
	    
	    dtostrf(d1_p[0],5,2,d1_send);
	    dtostrf(d1_p[1],5,2,str_aux);    
	    strcat(d1_send," ");
	    strcat(d1_send,str_aux);

      Serial.print("Sending to other node: ");
      Serial.println(d1_send);
	    
	    int error = i2c->msgSend(1,1,2,d1_send); // Enum for flags (perhaps), save other address somewhere... + Check errors??
      if(error != 0) Serial.println("Data not sent");   	    
	    j++;
	}
  }
  
  return d1[0];
   
}
