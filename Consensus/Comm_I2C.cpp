#include "Comm_I2C.h"

Comm_I2C::Comm_I2C(int _addr){
	addr = _addr;
}

void Comm_I2C::msgAnalyse(int id, String data_str){

}

int Comm_I2C::msgSend(int id, int dest_addr, String data_str){

	Wire.beginTransmission(dest_addr);
	Wire.write(id);
	Wire.write(" ");
	Wire.write(addr);
	Wire.write(" ");
	Wire.write(data_str);
	
	return Wire.endTransmission(); // Returns 0 if the msg was sent successfully
	
}

int Comm_I2C::msgBroadcast(int id, String data_str){

	error = msgSend(id,0,data_str);
	
	return error; // Returns 0 if the msg was sent successfully	
}


const char* Comm_I2C::getConsensusData(){

  return consensus_data.c_str();
}

int Comm_I2C::getAddr() const{

  return addr;
}

String Comm_I2C::floatToString(float num){

	char* str;
	dtostrf(str,5,2,num);
	String str2(str);

	return str2;

}

