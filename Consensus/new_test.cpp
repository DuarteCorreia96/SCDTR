#include "new_consensus.h"

using namespace std;

int ndev = 3;

// The external illumance is set to 0
int main(){

  const float c = 1;
  float L1 = 150;
  float L2 = 150;
  float L3 = 50;

  //float n1_k[2] = {3.17,0.73};
  //float n2_k[2] = {0.56,3.16};
  float n1_k[3] = {3.17,0.73,0.37};
  float n2_k[3] = {0.56,3.16,0.49};
  float n3_k[3] = {0.36,0.74,3.01};

  Consensus n1(1,c,L1,n1_k);
  Consensus n2(2,c,L2,n2_k);
  Consensus n3(3,c,L3,n3_k);

  float* d_matrix[MAX_LUM];

  for(int j = 0; j < 20; j++){

    //cout << "Iter " << j << endl;

    n1.primal();
    d_matrix[0] = n1.d_best;
    n2.primal();
    d_matrix[1] = n2.d_best;
    n3.primal();
    d_matrix[2] = n3.d_best;

    n1.consensus_update(d_matrix);   
    n2.consensus_update(d_matrix);
    n3.consensus_update(d_matrix);
  
  }
  
  cout << endl;
  n1.printResults();
  n1.printLux();
  cout << endl;
  n2.printResults();
  n2.printLux();
  cout << endl;
  n3.printResults();
  n3.printLux();

}