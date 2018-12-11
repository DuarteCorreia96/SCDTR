#include "Comm_I2C.h"

Comm_I2C::Comm_I2C(int _addr){
	addr = _addr;
}

const char* Comm_I2C::getConsensusData(){

  return consensus_data.c_str();
}

int Comm_I2C::getAddr() const{

  return addr;
}


