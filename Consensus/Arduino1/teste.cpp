#include "consensus.h"

using namespace std;

int main(){

  const float c = 1;
  float L1 = 70;
  float L2 = 150; 

  Consensus n1(c,L1,3.17,0.73);
  Consensus n2(c,L2,3.16,0.56);

  for(int j = 0; j < 20; j++){

    //cout << "Iter " << j << endl;

    n1.primal();
    n2.primal();

    n1.consensus_update(n2.d_out);   
    n2.consensus_update(n1.d_out);

  }

  cout << endl;
  n1.printResults();
  cout << endl;
  n2.printResults();

}